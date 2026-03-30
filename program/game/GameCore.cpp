#include "Game.h"

#include <cstring>

#include "DxLib.h"

#include "GameConstants.h"
#include "SoundID.h"
#include "SoundManager.h"

// キー入力のトリガー判定
bool Game::IsKeyTrigger(int keyCode) const
{
    return key_[keyCode] == 1 && prev_key_[keyCode] == 0;
}

// ゲーム全体の初期化
Game::Game()
    : map_()
    , player_(map_)
    , ring_()
{
    logo_font_ = CreateFontToHandle(
        "Yu Gothic UI",
        FontConst::kLogoSize,
        FontConst::kLogoThickness);

    press_font_ = CreateFontToHandle(
        "Yu Gothic UI",
        FontConst::kPressSize,
        FontConst::kPressThickness);

    sub_logo_font_ = CreateFontToHandle(
        "Yu Gothic UI",
        FontConst::kSubLogoSize,
        FontConst::kSubLogoThickness,
        DX_FONTTYPE_ANTIALIASING_EDGE);

    tutorial_big_font_ = CreateFontToHandle(
        "Yu Gothic UI",
        FontConst::kTutorialBigSize,
        FontConst::kTutorialBigThickness,
        DX_FONTTYPE_ANTIALIASING_EDGE);

    skip_font_ = CreateFontToHandle(
        "Yu Gothic UI",
        FontConst::kSkipSize,
        FontConst::kSkipThickness,
        DX_FONTTYPE_ANTIALIASING_EDGE);

    guide_title_font_ = CreateFontToHandle(
        "Yu Gothic UI",
        FontConst::kGuideTitleSize,
        FontConst::kGuideTitleThickness,
        DX_FONTTYPE_ANTIALIASING_EDGE);

    guide_font_ = CreateFontToHandle(
        "Yu Gothic UI",
        FontConst::kGuideSize,
        FontConst::kGuideThickness,
        DX_FONTTYPE_ANTIALIASING_EDGE);

    guide_big_font_ = CreateFontToHandle(
        "Yu Gothic UI",
        FontConst::kGuideBigSize,
        FontConst::kGuideBigThickness,
        DX_FONTTYPE_ANTIALIASING_EDGE);

    count_font_ = CreateFontToHandle(
        "Yu Gothic UI",
        FontConst::kCountSize,
        FontConst::kCountThickness,
        DX_FONTTYPE_ANTIALIASING_EDGE);

    ResetGame();
    scene_ = SceneType::TITLE;
}

// ゲーム全体のリセット
void Game::ResetGame()
{
    ring_ = Ring();
    map_ = Map();
    player_ = Player(map_);

    current_alphabet_ = GameConst::kAlphabetStart;
    prev_alphabet_ = GameConst::kNoAlphabet;

    enemies_a_.clear();
    enemies_b_.clear();
    chasers_.clear();
    current_enemies_ = &enemies_a_;

    portals_.clear();
    items_.clear();

    enemy_move_delay_ = GameConst::kGameStartEnemyMoveDelay;
    next_enemy_move_delay_ = GameConst::kGameStartNextEnemyMoveDelay;
    delay_item_timer_ = 0;
    alphabet_boost_count_ = 0;

    is_alt_world_ = false;
    map_.SetAltWorld(false);

    has_generated_stage_pair_ = false;

    is_clear_ = false;
    clear_time_ = 0.0;

    signal_manager_.ClearAll();
}

// プレイ状態を保存
void Game::SaveGameState()
{
    saved_state_.playerTileX = player_.GetTilePosX();
    saved_state_.playerTileY = player_.GetTilePosY();

    saved_state_.ringTileX = ring_.GetTilePosX();
    saved_state_.ringTileY = ring_.GetTilePosY();

    saved_state_.isAltWorld = is_alt_world_;

    saved_state_.enemiesA = enemies_a_;
    saved_state_.enemiesB = enemies_b_;

    saved_state_.currentAlphabet = current_alphabet_;
    saved_state_.prevAlphabet = prev_alphabet_;
    saved_state_.clearTime = clear_time_;

    has_saved_data_ = true;
}

// プレイ状態を読み込む
void Game::LoadGameState()
{
    player_.SetTilePos(saved_state_.playerTileX, saved_state_.playerTileY);
    ring_.SetTilePos(saved_state_.ringTileX, saved_state_.ringTileY);

    is_alt_world_ = saved_state_.isAltWorld;
    map_.SetAltWorld(is_alt_world_);

    enemies_a_ = saved_state_.enemiesA;
    enemies_b_ = saved_state_.enemiesB;

    if (is_alt_world_) {
        current_enemies_ = &enemies_b_;
    }
    else {
        current_enemies_ = &enemies_a_;
    }

    ApplyCurrentGeneratedMap();

    current_alphabet_ = saved_state_.currentAlphabet;
    prev_alphabet_ = saved_state_.prevAlphabet;
    clear_time_ = saved_state_.clearTime;
}

// カウントダウン開始
void Game::StartCountDown()
{
    SoundManager::StopBGM();
    count_ = GameConst::kInitialCountdown;
    count_start_time_ = GetNowHiPerformanceCount();
    scene_ = SceneType::COUNTDOWN;
}

// プレイ開始
void Game::StartPlay()
{
    SetupGeneratedStage();

    scene_ = SceneType::PLAY;
    SoundManager::PlayBGM(BGM_PLAY);
}

// 出現メッセージを表示する
void Game::ShowSpawnMessage(const char* message)
{
    strncpy_s(
        spawn_message_,
        sizeof(spawn_message_),
        message,
        _TRUNCATE);

    spawn_message_timer_ = GameConst::kSpawnMessageDurationFrames;
}

// ゲーム全体の更新
void Game::Update()
{
    memcpy(prev_key_, key_, GameConst::kKeyBufferSize);
    GetHitKeyStateAll(key_);

    switch (scene_) {
    case SceneType::TITLE:
        UpdateTitle();
        break;

    case SceneType::TUTORIAL:
        UpdateTutorial();
        break;

    case SceneType::GUIDE:
        UpdateGuide();
        break;

    case SceneType::COUNTDOWN:
        UpdateCountDown();
        break;

    case SceneType::PLAY:
        UpdatePlay();
        break;

    case SceneType::RESUME_MENU:
        UpdateResumeMenu();
        break;

    case SceneType::CLEAR:
        UpdateClear();
        break;
    }
}

// ゲーム全体の描画
void Game::Draw()
{
    switch (scene_) {
    case SceneType::TITLE:
        DrawTitle();
        break;

    case SceneType::TUTORIAL:
        DrawTutorial();
        break;

    case SceneType::GUIDE:
        DrawGuide();
        break;

    case SceneType::COUNTDOWN:
        DrawCountDown();
        break;

    case SceneType::PLAY:
        DrawPlay();
        break;

    case SceneType::RESUME_MENU:
        DrawResume();
        break;

    case SceneType::CLEAR:
        DrawClear();
        break;
    }
}