#include "Game.h"

#include <cstdlib>
#include <cstring>

#include "DxLib.h"

#include "GameConstants.h"
#include "SoundID.h"
#include "SoundManager.h"
#include "SignalManager.h"

// タイトル画面
void Game::UpdateTitle()
{
    ++title_frame_;

    SoundManager::PlayBGM(BGM_TITLE);

    if (scene_ != SceneType::TITLE) {
        return;
    }

    if (isExitPopup_)
    {
        if (IsKeyTrigger(KEY_INPUT_LEFT) || IsKeyTrigger(KEY_INPUT_A))
        {
            exitCursor_ = 0;
        }

        if (IsKeyTrigger(KEY_INPUT_RIGHT) || IsKeyTrigger(KEY_INPUT_D))
        {
            exitCursor_ = 1;
        }

        if (IsKeyTrigger(KEY_INPUT_ESCAPE))
        {
            isExitPopup_ = false;
            SoundManager::PlaySE(SE_DECIDE);
            return;
        }

        if (IsKeyTrigger(KEY_INPUT_RETURN))
        {
            SoundManager::PlaySE(SE_DECIDE);

            if (exitCursor_ == 0)
            {
                DxLib_End();
                exit(0);
            }
            else
            {
                isExitPopup_ = false;
            }
            return;
        }

        return;
    }

    if (IsKeyTrigger(KEY_INPUT_ESCAPE))
    {
        isExitPopup_ = true;
        exitCursor_ = 1;
        SoundManager::PlaySE(SE_DECIDE);
        return;
    }

    if (!IsKeyTrigger(KEY_INPUT_RETURN)) {
        return;
    }

    SoundManager::PlaySE(SE_DECIDE);

    if (has_saved_data_) {
        scene_ = SceneType::RESUME_MENU;
    }
    else {
        tutorial_manager_.Init(*this);

        if (is_first_launch_) {
            scene_ = SceneType::TUTORIAL;
        }
        else {
            scene_ = SceneType::GUIDE;
        }
    }
}

// チュートリアル画面
void Game::UpdateTutorial()
{
    UpdatePlay();

    if (IsKeyTrigger(KEY_INPUT_R)) {
        is_skip_popup_ = true;
        skip_index_ = 0;
        return;
    }

    if (is_skip_popup_) {
        if (IsKeyTrigger(KEY_INPUT_A)) {
            skip_index_ = 0;
        }
        else if (IsKeyTrigger(KEY_INPUT_D)) {
            skip_index_ = 1;
        }

        if (IsKeyTrigger(KEY_INPUT_RETURN)) {
            if (skip_index_ == 0) {
                signal_manager_.Emit(TutorialSignal::kTutorialSkipped);
                tutorial_manager_.SkipReset();
                scene_ = SceneType::GUIDE;
            }

            is_skip_popup_ = false;
        }
        return;
    }

    tutorial_manager_.Update(*this);

    if (tutorial_manager_.IsFinished()) {
        ResetGame();
        StartCountDown();
    }
}

// ルール説明画面
void Game::UpdateGuide()
{
    ++guide_frame_;

    if (IsKeyTrigger(KEY_INPUT_RETURN)) {
        SoundManager::PlaySE(SE_DECIDE);
        ResetGame();
        StartCountDown();
    }
}

// カウントダウン画面
void Game::UpdateCountDown()
{
    const long long now = GetNowHiPerformanceCount();

    if (now - count_start_time_ < GameConst::kCountdownInterval) {
        return;
    }

    --count_;
    count_start_time_ = now;

    if (count_ > 0) {
        SoundManager::PlaySE(SE_COUNT);
    }
    else if (count_ == 0) {
        SoundManager::PlaySE(SE_START);
    }
    else {
        StartPlay();
    }
}

// プレイ画面
void Game::UpdatePlay()
{
    if (CheckHitKey(KEY_INPUT_B)) {
        ResetGame();
        StartCountDown();
        return;
    }

    ring_.Update();

    for (auto& item : items_) {
        item.Update();
    }

    for (auto& portal : portals_) {
        portal.Update();
    }

    if (portal_cooldown_ > 0) {
        --portal_cooldown_;
    }

    if (!tutorial_manager_.IsInputLocked() && !is_skip_popup_) {
        if (!is_warping_) {
            player_.Update(map_);
        }
    }

    CheckItem();

    if (ring_.CheckHit(player_)) {
        signal_manager_.Emit(TutorialSignal::kRingEntered);

        if (player_.IsZ()) {
            const long long now = GetNowHiPerformanceCount();
            clear_time_ =
                (now - start_time_) / static_cast<double>(CommonConst::kOneSecondMicroseconds);

            SoundManager::StopBGM();
            SoundManager::PlayBGM(BGM_CLEAR);

            is_clear_ = true;
            scene_ = SceneType::CLEAR;
        }
        else {
            if (scene_ == SceneType::PLAY) {
                ToggleWorld();
            }

            map_.SetAltWorld(is_alt_world_);

            UpdateAlphabet();
            CheckSpawnPatrolEnemy();

            RelocateRingFromPlayerInRange(10, 25);
            SoundManager::PlaySE(SE_DECIDE);
        }
    }

    for (auto& enemy : *current_enemies_) {
        if (ring_.CheckHit(enemy)) {
            RelocateRingInRangeFromTile(
                enemy.GetTilePosX(),
                enemy.GetTilePosY(),
                10,
                15);
            SoundManager::PlaySE(SE_DECIDE);
        }
    }

    for (auto& chaser : chasers_) {
        if (ring_.CheckHit(chaser)) {
            RelocateRingInRangeFromTile(
                chaser.GetTilePosX(),
                chaser.GetTilePosY(),
                10,
                15);
            SoundManager::PlaySE(SE_DECIDE);
        }
    }

    CheckPortal();

    if (is_warping_) {
        const long long now = GetNowHiPerformanceCount();

        if (now - warp_start_time_ >= GameConst::kWaitMicroseconds) {
            const int pair = portals_[warp_from_index_].GetPairIndex();

            player_.SetTilePos(
                portals_[pair].GetTilePosX(),
                portals_[pair].GetTilePosY());

            is_warping_ = false;
            portal_cooldown_ = GameConst::kPortalCooldownFrames;
        }
    }

    if (scene_ == SceneType::PLAY && IsKeyTrigger(KEY_INPUT_ESCAPE)) {
        SaveGameState();
        scene_ = SceneType::TITLE;
    }

    UpdateSpawnMessage();
}

// 出現メッセージ
void Game::UpdateSpawnMessage()
{
    if (spawn_message_timer_ > 0) {
        --spawn_message_timer_;
    }

    if (spawn_message_timer_ <= 0) {
        spawn_message_[0] = '\0';
        spawn_message_timer_ = 0;
    }
}

// ゲーム再開メニュー
void Game::UpdateResumeMenu()
{
    if (IsKeyTrigger(KEY_INPUT_W)) {
        resume_index_ = 0;
    }
    if (IsKeyTrigger(KEY_INPUT_S)) {
        resume_index_ = 1;
    }

    if (!IsKeyTrigger(KEY_INPUT_RETURN)) {
        return;
    }

    if (resume_index_ == 0) {
        LoadGameState();
        scene_ = SceneType::PLAY;
        SoundManager::StopBGM();
        SoundManager::PlayBGM(BGM_PLAY);
    }
    else {
        has_saved_data_ = false;
        ResetGame();
        scene_ = SceneType::GUIDE;
    }
}

// クリア画面
void Game::UpdateClear()
{
    if (CheckHitKey(KEY_INPUT_B)) {
        SoundManager::PlaySE(SE_DECIDE);
        scene_ = SceneType::TITLE;
    }

    if (CheckHitKey(KEY_INPUT_RETURN)) {
        SoundManager::PlaySE(SE_DECIDE);
        ResetGame();
        StartCountDown();
    }
}