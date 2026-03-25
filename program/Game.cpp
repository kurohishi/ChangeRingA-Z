#include <cstdio>
#include <cstring>
#include <queue>

#include "DxLib.h"

#include "Game.h"
#include "Constants.h"
#include "SoundID.h"
#include "SoundManager.h"

namespace
{
    // ===== 敵の巡回ルート =====

    // A世界
    const std::vector<PatrolPoint> kRouteA1 = {
        {3, 1}, {3, 3}, {7, 3}, {7, 5}, {10, 5},
        {10, 7}, {5, 7}, {5, 5}, {1, 5}, {1, 1}
    };

    const std::vector<PatrolPoint> kRouteA2 = {
        {17, 9}, {16, 9}, {16, 11}, {18, 11}, {18, 9}
    };

    // B世界
    const std::vector<PatrolPoint> kRouteB1 = {
        {1, 7}, {1, 9}, {4, 9}, {4, 11}, {6, 11}, {6, 7}
    };

    const std::vector<PatrolPoint> kRouteB2 = {
        {18, 9}, {14, 9}, {14, 11}, {12, 11}, {12, 13}, {18, 13}
    };
}

// リングから各タイルまでの距離マップを作る
void BuildDistanceMapTile(
    int goal_tile_x,
    int goal_tile_y,
    const Map& map,
    int dist[Map::H][Map::W]);

// ===== 入力 =====

// 指定したキーが今フレームで押された瞬間かどうか
bool Game::IsKeyTrigger(int keyCode) const
{
    return key_[keyCode] == 1 && prev_key_[keyCode] == 0;
}

// ==============================
//                       ゲーム本体
// ==============================

// コンストラクタ：ゲーム全体の初期化
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

// ゲーム状態の初期化
void Game::ResetGame()
{
    ring_ = Ring();
    map_ = Map();
    player_ = Player(map_);

    ring_.RelocateFar(player_, map_);

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

    is_clear_ = false;
    clear_time_ = 0.0;

    signal_manager_.ClearAll();
}

// チュートリアルステージの初期配置
void Game::SetupTutorialStage()
{
    map_.UseTutorialMapA();
    map_.SetAltWorld(false);
    is_alt_world_ = false;

    player_.SetTilePos(
        TutorialConst::kPlayerStartX,
        TutorialConst::kPlayerStartY);

    ring_.SetTilePos(
        TutorialConst::kRingStartX,
        TutorialConst::kRingStartY);

    ring_.SetNextTilePos(
        TutorialConst::kRingNextX,
        TutorialConst::kRingNextY);

    portals_.clear();
    portals_.emplace_back(
        TutorialConst::kPortalAX,
        TutorialConst::kPortalAY,
        TutorialConst::kPortalAPair);
    portals_.emplace_back(
        TutorialConst::kPortalBX,
        TutorialConst::kPortalBY,
        TutorialConst::kPortalBPair);

    chasers_.clear();
    items_.clear();

    signal_manager_.ClearAll();
}

// ゲーム状態の保存
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

// ゲーム状態の読み込み
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
        map_.UseMapB();
    }
    else {
        current_enemies_ = &enemies_a_;
        map_.UseMapA();
    }

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
    scene_ = SceneType::PLAY;
    SoundManager::PlayBGM(BGM_PLAY);
}

// ==============================
//              ゲーム全体の更新
// ==============================

void Game::Update()
{
    // キー入力の更新
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

// ===== 各ゲーム画面の更新 =====

// タイトル画面
void Game::UpdateTitle()
{
    ++title_frame_;

    SoundManager::PlayBGM(BGM_TITLE);

    if (scene_ != SceneType::TITLE) {
        return;
    }

    if (!CheckHitKey(KEY_INPUT_RETURN)) {
        return;
    }

    // セーブデータがあるときは続きから、ないときはチュートリアルから開始
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

    SoundManager::PlaySE(SE_DECIDE);
}

// チュートリアル画面
void Game::UpdateTutorial()
{
    UpdatePlay();

    // スキップポップアップ表示
    if (IsKeyTrigger(KEY_INPUT_R)) {
        is_skip_popup_ = true;
        skip_index_ = 0;
        return;
    }

    // ポップアップ表示中の入力処理
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

    // チュートリアル進行更新
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
    // デバッグ用：Bキーでリセット
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

            ring_.RelocateFar(player_, map_);
            SoundManager::PlaySE(SE_DECIDE);
        }
    }

    for (auto& enemy : *current_enemies_) {
        if (ring_.CheckHit(enemy)) {
            ring_.RelocateFar(enemy, map_);
            SoundManager::PlaySE(SE_DECIDE);
        }
    }

    for (auto& chaser : chasers_) {
        if (ring_.CheckHit(chaser)) {
            ring_.RelocateFar(chaser, map_);
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
}

// ゲーム再開画面
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

// ゲームクリア画面
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

// ==============================
//              ギミック関連の処理
// ==============================

// 追跡敵のランダム生成
void Game::AddChaseEnemies()
{
    int pos_x = 0;
    int pos_y = 0;

    do {
        pos_x = GetRand(Map::W - 1);
        pos_y = GetRand(Map::H - 1);
    } while (!map_.IsWalkableTile(pos_x, pos_y));

    chasers_.emplace_back(pos_x, pos_y);
}

// ポータルの追加配置
void Game::AddPortals()
{
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;

    // 1つ目のポータル
    do {
        x1 = GetRand(Map::W - 1);
        y1 = GetRand(Map::H - 1);
    } while (!map_.IsWalkableTile(x1, y1));

    // 2つ目のポータル
    do {
        x2 = GetRand(Map::W - 1);
        y2 = GetRand(Map::H - 1);
    } while (!map_.IsWalkableTile(x2, y2) || (x1 == x2 && y1 == y2));

    portals_.emplace_back(x1, y1, 1);
    portals_.emplace_back(x2, y2, 0);
}

// ポータルでワープ可能かを判定する
void Game::CheckPortal()
{
    if (is_warping_) {
        return;
    }

    bool hit_now = false;

    for (int i = 0; i < static_cast<int>(portals_.size()); ++i)
    {
        if (portals_[i].CheckHit(player_)) {
            hit_now = true;

            // 前フレームでは乗っておらず、今フレームで初めて乗ったときだけワープ開始
            if (!player_on_portal_) {
                is_warping_ = true;
                warp_from_index_ = i;
                warp_start_time_ = GetNowHiPerformanceCount();
                SoundManager::PlaySE(SE_PORTAL);
                signal_manager_.Emit(TutorialSignal::kPortalUsed);
            }
            break;
        }
    }

    player_on_portal_ = hit_now;
}

// アイテムの取得判定と効果適用
void Game::CheckItem()
{
    for (int i = 0; i < static_cast<int>(items_.size()); ++i) {
        if (!items_[i].CheckHit(player_)) {
            continue;
        }

        if (items_[i].GetType() == ItemType::SLOW_ENEMY) {
            // このターンではなく、次の敵行動から反映する
            next_enemy_move_delay_++;
            delay_item_timer_ = GameConst::kItemEffectDurationFrames;
            SoundManager::PlaySE(SE_ITEM_ACCEL);
        }
        else if (items_[i].GetType() == ItemType::BOOST_ALPHABET) {
            // 次のリング取得時の進みを加速
            alphabet_boost_count_++;
            SoundManager::PlaySE(SE_ITEM_SKIP);
        }

        signal_manager_.Emit(TutorialSignal::kItemPicked);
        items_.erase(items_.begin() + i);
        break;
    }
}

// 指定した種類のアイテムがあるか
bool Game::HasItem(ItemType type) const
{
    for (const auto& item : items_) {
        if (item.GetType() == type) {
            return true;
        }
    }
    return false;
}

// 指定した種類のアイテム数を返す
int Game::CountItems(ItemType type) const
{
    int count = 0;

    for (const auto& item : items_) {
        if (item.GetType() == type) {
            ++count;
        }
    }
    return count;
}

// ===== アルファベット・敵出現 =====

// アルファベットを進める（リング取得時）
void Game::UpdateAlphabet()
{
    prev_alphabet_ = current_alphabet_;

    int step = GameConst::kAlphabetNormalStep;

    if (alphabet_boost_count_ > 0) {
        step = GameConst::kAlphabetBoostStep;
        alphabet_boost_count_--;
        SoundManager::PlaySE(SE_SKIP_WORD);
    }

    for (int i = 0; i < step; ++i) {
        if (current_alphabet_ < GameConst::kAlphabetGoal) {
            player_.LevelUp();
            current_alphabet_++;
        }
    }
}

// アルファベットを1つ戻す（敵接触時）
void Game::DownAlphabet()
{
    prev_alphabet_ = current_alphabet_;

    if (current_alphabet_ > GameConst::kAlphabetStart) {
        player_.LevelDown();
        current_alphabet_--;
        SoundManager::PlaySE(SE_BACK_WORD);
    }
}

// アルファベットの変化で敵やアイテムを出現させる（各1度きり）
void Game::CheckSpawnPatrolEnemy()
{
    if (scene_ != SceneType::PLAY) {
        return;
    }

    if (current_alphabet_ == prev_alphabet_) {
        return;
    }

    bool spawned = false;

    // 巡回敵：1体目追加
    if (prev_alphabet_ < GameConst::kSpawnPatrol1At &&
        current_alphabet_ >= GameConst::kSpawnPatrol1At) {
        if (enemies_a_.size() < GameConst::kFirstEnemyCount &&
            enemies_b_.size() < GameConst::kFirstEnemyCount) {
            enemies_a_.emplace_back(kRouteA1);
            enemies_b_.emplace_back(kRouteB1);
            spawned = true;
        }
    }

    // 巡回敵：2体目追加
    if (prev_alphabet_ < GameConst::kSpawnPatrol2At &&
        current_alphabet_ >= GameConst::kSpawnPatrol2At) {
        if (enemies_a_.size() < GameConst::kSecondEnemyCount &&
            enemies_b_.size() < GameConst::kSecondEnemyCount) {
            enemies_a_.emplace_back(kRouteA2);
            enemies_b_.emplace_back(kRouteB2);
            AddPortals();
            spawned = true;
        }
    }

    // 追跡敵：1体目と加速アイテム追加
    if (prev_alphabet_ < GameConst::kSpawnChaser1At &&
        current_alphabet_ >= GameConst::kSpawnChaser1At) {
        if (chasers_.size() < GameConst::kFirstEnemyCount) {
            AddChaseEnemies();
            spawned = true;
        }

        if (!HasItem(ItemType::SLOW_ENEMY) &&
            enemy_move_delay_ < GameConst::kNormalEnemyMoveDelay) {
            items_.emplace_back(Item::CreateRandom(map_, ItemType::SLOW_ENEMY));
            spawned = true;
        }
    }

    // 促進アイテム追加
    if (current_alphabet_ >= GameConst::kSpawnBoostItemAt) {
        if (!HasItem(ItemType::BOOST_ALPHABET)) {
            items_.emplace_back(Item::CreateRandom(map_, ItemType::BOOST_ALPHABET));
            spawned = true;
        }
    }

    // 追跡敵：2体目と加速アイテム追加
    if (prev_alphabet_ < GameConst::kSpawnChaser2At &&
        current_alphabet_ >= GameConst::kSpawnChaser2At) {
        if (chasers_.size() < GameConst::kSecondEnemyCount) {
            AddChaseEnemies();
            spawned = true;
        }

        if (enemy_move_delay_ < GameConst::kNormalEnemyMoveDelay) {
            if (CountItems(ItemType::SLOW_ENEMY) < GameConst::kSecondEnemyCount) {
                items_.emplace_back(Item::CreateRandom(map_, ItemType::SLOW_ENEMY));
                spawned = true;
            }
        }
        else if (enemy_move_delay_ == GameConst::kNormalEnemyMoveDelay + 1) {
            if (!HasItem(ItemType::SLOW_ENEMY)) {
                items_.emplace_back(Item::CreateRandom(map_, ItemType::SLOW_ENEMY));
                spawned = true;
            }
        }
    }

    // ワープポータル追加
    if (prev_alphabet_ < GameConst::kSpawnPortalAt &&
        current_alphabet_ >= GameConst::kSpawnPortalAt) {
        AddPortals();
        spawned = true;
    }

    // 追跡敵：3体目追加
    if (prev_alphabet_ < GameConst::kSpawnChaser3At &&
        current_alphabet_ >= GameConst::kSpawnChaser3At) {
        if (chasers_.size() < GameConst::kThirdChaserCount) {
            AddChaseEnemies();
            spawned = true;
        }
    }

    if (spawned) {
        SoundManager::PlaySE(SE_ENEMY_SPAWN);
    }
}

// 世界を切り替える
void Game::ToggleWorld()
{
    // リングが両方の世界で通行可能な位置にあるときだけ切り替える
    if (!map_.CanSwitchRingPos(ring_.GetTilePosX(), ring_.GetTilePosY())) {
        return;
    }

    is_alt_world_ = !is_alt_world_;
    map_.SetAltWorld(is_alt_world_);

    if (is_alt_world_) {
        map_.UseMapB();
        current_enemies_ = &enemies_b_;
    }
    else {
        map_.UseMapA();
        current_enemies_ = &enemies_a_;
    }

    portals_.clear();
    items_.clear();
    chasers_.clear();

    // ステージ切り替え時のギミック再配置
    if (current_alphabet_ >= GameConst::kSpawnPatrol2At) {
        AddPortals();
    }

    if (current_alphabet_ >= GameConst::kSpawnChaser1At) {
        AddChaseEnemies();

        if (enemy_move_delay_ == GameConst::kGameStartEnemyMoveDelay) {
            items_.emplace_back(Item::CreateRandom(map_, ItemType::SLOW_ENEMY));
        }
    }

    if (current_alphabet_ >= GameConst::kSpawnBoostItemAt) {
        items_.emplace_back(Item::CreateRandom(map_, ItemType::BOOST_ALPHABET));
    }

    if (current_alphabet_ >= GameConst::kSpawnChaser2At) {
        AddChaseEnemies();

        if (enemy_move_delay_ == GameConst::kNormalEnemyMoveDelay) {
            items_.emplace_back(Item::CreateRandom(map_, ItemType::SLOW_ENEMY));
            items_.emplace_back(Item::CreateRandom(map_, ItemType::SLOW_ENEMY));
        }
    }

    if (current_alphabet_ >= GameConst::kSpawnPortalAt) {
        AddPortals();
    }

    if (current_alphabet_ >= GameConst::kSpawnChaser3At) {
        AddChaseEnemies();
    }

    SoundManager::PlaySE(SE_PORTAL);
    signal_manager_.Emit(TutorialSignal::kWorldShifted);
}

// ==============================
//              ゲーム全体の描画
// ==============================

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

// ===== 各ゲーム画面の描画  =====

// タイトル画面
void Game::DrawTitle() const
{
    DrawBox(
        0,
        0,
        GameConst::kScreenWidth,
        GameConst::kScreenHeight,
        GetColor(40, 40, 50),
        TRUE);

    for (int y = 0; y < GameConst::kScreenHeight; y += GameConst::kTitleGridCellH)
    {
        for (int x = 0; x < GameConst::kScreenWidth; x += GameConst::kTitleGridCellW)
        {
            DrawBox(
                x,
                y,
                x + GameConst::kTitleGridBoxW,
                y + GameConst::kTitleGridBoxH,
                GetColor(60, 60, 70),
                FALSE
            );
        }
    }

    for (int i = 0; i < GameConst::kTitleCircleCount; ++i) {
        const int x =
            (title_frame_ * GameConst::kTitleFrameSpeedX +
                i * GameConst::kTitleBgCircleSpacingX) % GameConst::kScreenWidth;
        const int y =
            (i * GameConst::kTitleBgCircleSpacingY + title_frame_) % GameConst::kScreenHeight;

        const int gray = GameConst::kTitleBgGrayBase +
            static_cast<int>(
                GameConst::kTitleBgGrayRange *
                (sin((title_frame_ + i * GameConst::kTitleBgWaveOffset) *
                    GameConst::kTitleBgWaveSpeed) + 1.0) * 0.5);

        DrawCircle(
            x,
            y,
            GameConst::kTitleBgCircleRadius,
            GetColor(gray, gray, gray),
            TRUE);
    }

    const char* logo = "ChangeRing O";
    int draw_x = GameConst::kTitleLogoStartX;
    const int base_y = GameConst::kTitleLogoBaseY;

    for (int i = 0; logo[i] != '\0'; ++i) {
        char c[2] = { logo[i], '\0' };
        const int draw_y = base_y + static_cast<int>(
            sin(title_frame_ * GameConst::kTitleLogoWaveSpeed +
                i * GameConst::kTitleLogoWaveOffset) *
            GameConst::kTitleLogoWaveHeight);

        const int width = GetDrawStringWidthToHandle(c, 1, logo_font_);

        DrawStringToHandle(
            draw_x + GameConst::kTitleLogoShadowOffset,
            draw_y + GameConst::kTitleLogoShadowOffset,
            c,
            GetColor(120, 0, 120),
            logo_font_);

        for (int ox = -GameConst::kTitleLogoOutlineRadius;
            ox <= GameConst::kTitleLogoOutlineRadius;
            ++ox) {
            for (int oy = -GameConst::kTitleLogoOutlineRadius;
                oy <= GameConst::kTitleLogoOutlineRadius;
                ++oy) {
                if (ox * ox + oy * oy <= GameConst::kTitleLogoOutlineDist2) {
                    DrawStringToHandle(
                        draw_x + ox,
                        draw_y + oy,
                        c,
                        GetColor(255, 255, 255),
                        logo_font_);
                }
            }
        }

        const double t = (title_frame_ + i * 10) * 0.05;
        const double wave = (sin(t) + 1.0) * 0.5;

        const int r = static_cast<int>(200 * wave);
        const int g = 200;
        const int b = static_cast<int>(200 * (1.0 - wave));

        DrawStringToHandle(draw_x, draw_y, c, GetColor(r, g, b), logo_font_);
        draw_x += width;
    }

    if ((title_frame_ / GameConst::kTitleBlinkInterval) % 2 == 0) {
        DrawStringToHandle(
            GameConst::kTitlePressX,
            GameConst::kTitlePressY,
            "Press Enter!",
            GetColor(255, 255, 255),
            press_font_);
    }
}

// チュートリアル画面
void Game::DrawTutorial()
{
    DrawPlay();
    tutorial_manager_.Draw();

    if (!is_skip_popup_) {
        DrawStringToHandle(
            GameConst::kSkipHintX,
            GameConst::kSkipHintY,
            "[R]でスキップ",
            GetColor(255, 255, 255),
            skip_font_);
    }

    if (is_skip_popup_) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, GameConst::kSkipOverlayAlpha);
        DrawBox(0, 0, GameConst::kScreenWidth, GameConst::kScreenHeight, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

        DrawBox(
            GameConst::kSkipPopupX1,
            GameConst::kSkipPopupY1,
            GameConst::kSkipPopupX2,
            GameConst::kSkipPopupY2,
            GetColor(30, 30, 50),
            TRUE);

        DrawBox(
            GameConst::kSkipPopupX1,
            GameConst::kSkipPopupY1,
            GameConst::kSkipPopupX2,
            GameConst::kSkipPopupY2,
            GetColor(200, 200, 255),
            FALSE);

        DrawString(
            GameConst::kSkipPopupTextX,
            GameConst::kSkipPopupTextY,
            "チュートリアルをスキップしますか？",
            GetColor(255, 255, 255));

        DrawString(
            GameConst::kSkipYesX,
            GameConst::kSkipYesY,
            "Yes",
            skip_index_ == 0 ? GetColor(255, 255, 0) : GetColor(200, 200, 200));

        DrawString(
            GameConst::kSkipNoX,
            GameConst::kSkipNoY,
            "No",
            skip_index_ == 1 ? GetColor(255, 255, 0) : GetColor(200, 200, 200));
    }
}

// ルール説明画面
void Game::DrawGuide() const
{
    DrawBox(0, 0, GameConst::kScreenWidth, GameConst::kScreenHeight, GetColor(15, 15, 25), TRUE);

    const char* title = "- GAME GUIDE -";
    const int width = GetDrawStringWidthToHandle(title, -1, guide_title_font_);
    const int x = GameConst::kScreenCenterX - width / 2;

    DrawStringToHandle(
        x + (GameConst::kGuideTitleYShadow - GameConst::kGuideTitleY),
        GameConst::kGuideTitleYShadow,
        title,
        GetColor(0, 0, 0),
        guide_title_font_);
    DrawStringToHandle(
        x,
        GameConst::kGuideTitleY,
        title,
        GetColor(255, 220, 80),
        guide_title_font_);

    DrawString(
        GameConst::kGuideTextX,
        GameConst::kGuideLine1Y,
        "① リング 〇に入ると、アルファベット『A』が進む（A→B→…）",
        GetColor(255, 255, 255));

    DrawString(
        GameConst::kGuideTextX,
        GameConst::kGuideLine2Y,
        "② 敵△と重なると、アルファベットが１つ戻る（C→B→A）",
        GetColor(255, 100, 255));

    DrawString(
        GameConst::kGuideTextX,
        GameConst::kGuideLine3Y,
        "③ アルファベット『Z』で〇に入るとゲームクリア！",
        GetColor(255, 255, 255));

    DrawString(
        GameConst::kGuideTextX,
        GameConst::kGuideLine4Y,
        "④ 〇が〈他のステージ〉と重なる位置にあれば、ステージも変わる",
        GetColor(255, 255, 100));

    if ((guide_frame_ / GameConst::kTitleBlinkInterval) % 2 == 0) {
        DrawStringToHandle(
            GameConst::kGuideStartX,
            GameConst::kGuideStartY,
            "Press Enter to Start",
            GetColor(190, 200, 255),
            guide_big_font_);
    }
}

// カウントダウン画面
void Game::DrawCountDown() const
{
    if (count_ > 0) {
        char buf[8];
        sprintf_s(buf, "%d", count_);

        const int width = GetDrawStringWidthToHandle(buf, 1, count_font_);
        const int x = GameConst::kScreenCenterX - width / 2;
        const int y = GameConst::kCountTextY;

        DrawStringToHandle(
            x + GameConst::kCountShadowOffset,
            y + GameConst::kCountShadowOffset,
            buf,
            GetColor(0, 0, 0),
            count_font_);
        DrawStringToHandle(
            x,
            y,
            buf,
            GetColor(255, 255, 255),
            count_font_);
    }
    else {
        const char* text = "Start!";
        const int width = GetDrawStringWidthToHandle(text, 6, count_font_);
        const int x = GameConst::kScreenCenterX - width / 2;
        const int y = GameConst::kCountTextY;

        DrawStringToHandle(
            x + GameConst::kCountShadowOffset,
            y + GameConst::kCountShadowOffset,
            text,
            GetColor(0, 0, 0),
            count_font_);
        DrawStringToHandle(
            x,
            y,
            text,
            GetColor(255, 255, 0),
            count_font_);
    }
}

// プレイ画面
void Game::DrawPlay()
{
    if (is_alt_world_) {
        DrawBox(0, 0, GameConst::kScreenWidth, GameConst::kScreenHeight, GetColor(10, 10, 25), TRUE);
    }
    else {
        DrawBox(0, 0, GameConst::kScreenWidth, GameConst::kScreenHeight, GetColor(255, 40, 50), TRUE);
    }

    map_.Draw();

    for (auto& portal : portals_) {
        portal.Draw();
    }

    for (auto& item : items_) {
        item.Draw();
    }

    ring_.Draw();
    player_.Draw();

    for (const auto& chaser : chasers_) {
        chaser.Draw();
    }

    if (player_.HasDrawPositionChanged()) {
        player_move_count_++;
        signal_manager_.Emit(TutorialSignal::kPlayerMoved);

        if (player_move_count_ >= enemy_move_delay_) {
            player_move_count_ = 0;

            for (auto& enemy : *current_enemies_) {
                enemy.Update(map_);
            }

            BuildDistanceMapTile(
                ring_.GetTilePosX(),
                ring_.GetTilePosY(),
                map_,
                dist_);

            for (auto& chaser : chasers_) {
                chaser.Update(dist_, map_);
            }

            // 敵の移動が終わったあとで次の遅延設定を反映
            enemy_move_delay_ = next_enemy_move_delay_;
        }

        // ===== 接触判定は毎回行う =====
        for (auto& enemy : *current_enemies_) {
            if (enemy.CheckHit(player_)) {
                DownAlphabet();
                signal_manager_.Emit(TutorialSignal::kPatrolEnemyHit);
                break;
            }
        }

        for (auto& chaser : chasers_) {
            if (chaser.CheckHit(player_)) {
                DownAlphabet();
                signal_manager_.Emit(TutorialSignal::kPatrolEnemyHit);
                break;
            }
        }
    }

    for (auto& enemy : *current_enemies_) {
        enemy.Draw();
    }

    player_.Draw();
    player_.CommitDrawPosition();
}

// ゲーム再開画面
void Game::DrawResume() const
{
    for (int y = 0; y < GameConst::kScreenHeight; ++y) {
        DrawLine(0, y, GameConst::kScreenWidth, y, GetColor(10, 10, 10));
    }

    DrawBox(
        GameConst::kResumeBoxX1,
        GameConst::kResumeBoxY1,
        GameConst::kResumeBoxX2,
        GameConst::kResumeBoxY2,
        GetColor(30, 30, 50),
        TRUE);

    DrawBox(
        GameConst::kResumeBoxX1,
        GameConst::kResumeBoxY1,
        GameConst::kResumeBoxX2,
        GameConst::kResumeBoxY2,
        GetColor(80, 80, 150),
        FALSE);

    DrawStringToHandle(
        GameConst::kResumeTitleX,
        GameConst::kResumeTitleY,
        "- ACCESS POINT -",
        GetColor(250, 220, 0),
        guide_title_font_);

    for (int i = 0; i < 2; ++i) {
        const int y = GameConst::kResumeItemY + i * GameConst::kResumeItemSpacingY;
        const char* text = (i == 0) ? "Continue" : "New Game";

        DrawStringToHandle(
            GameConst::kResumeItemX,
            y - GameConst::kResumeItemTextOffsetY,
            text,
            resume_index_ == i
            ? GetColor(255, 255, 255)
            : GetColor(160, 160, 200),
            guide_font_);
    }
}

// ゲームクリア画面
void Game::DrawClear()
{
    player_.Draw();

    DrawString(
        GameConst::kClearTitleX,
        GameConst::kClearTitleY,
        "GAME CLEAR!",
        GetColor(255, 255, 0));

    char buf[64];
    sprintf_s(buf, "CLEAR TIME : %.2f sec", clear_time_);
    DrawString(
        GameConst::kClearTimeX,
        GameConst::kClearTimeY,
        buf,
        GetColor(255, 255, 255));

    DrawString(
        GameConst::kClearHintX,
        GameConst::kClearHintY,
        "Enter : Retry   B : Title",
        GetColor(200, 200, 200));
}

// ==============================
//              距離マップの作成
// ==============================

// BFS でリングから各タイルへの距離を計算する
void BuildDistanceMapTile(
    int goal_tile_x,
    int goal_tile_y,
    const Map& map,
    int dist[Map::H][Map::W])
{
    for (int y = 0; y < Map::H; ++y) {
        for (int x = 0; x < Map::W; ++x) {
            dist[y][x] = GameConst::kDistUnreachable;
        }
    }

    if (goal_tile_x < 0 || goal_tile_y < 0 ||
        goal_tile_x >= Map::W || goal_tile_y >= Map::H) {
        return;
    }

    if (!map.IsWalkableTile(goal_tile_x, goal_tile_y)) {
        return;
    }

    std::queue<std::pair<int, int>> q;
    dist[goal_tile_y][goal_tile_x] = 0;
    q.push({ goal_tile_x, goal_tile_y });

    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();

        for (int i = 0; i < GameConst::kBfsDirectionCount; ++i) {
            const int nx = x + RingChaserConst::kDeltaX[i];
            const int ny = y + RingChaserConst::kDeltaY[i];

            if (nx < 0 || ny < 0 || nx >= Map::W || ny >= Map::H) {
                continue;
            }

            if (!map.IsWalkableTile(nx, ny)) {
                continue;
            }

            if (dist[ny][nx] != GameConst::kDistUnreachable) {
                continue;
            }

            dist[ny][nx] = dist[y][x] + 1;
            q.push({ nx, ny });
        }
    }
}

// ===== TutorialManager 用の公開関数 =====

bool Game::IsPlayerOnTile(int tile_x, int tile_y) const
{
    return player_.GetTilePosX() == tile_x &&
        player_.GetTilePosY() == tile_y;
}

bool Game::IsAnyChaserOnTile(int tile_x, int tile_y) const
{
    for (const auto& chaser : chasers_) {
        if (chaser.GetTilePosX() == tile_x &&
            chaser.GetTilePosY() == tile_y) {
            return true;
        }
    }
    return false;
}

char Game::GetCurrentAlphabet() const
{
    return current_alphabet_;
}

char Game::GetPreviousAlphabet() const
{
    return prev_alphabet_;
}

void Game::StartTutorialWorldShiftScene()
{
    portals_.clear();
    is_alt_world_ = true;
    map_.UseTutorialMapB();

    ring_.SetNextTilePos(1, 7);
    ring_.RelocateFar(player_, map_);

    current_enemies_ = &enemies_b_;
    enemies_b_.clear();
    enemies_b_.emplace_back(std::vector<PatrolPoint>{{14, 7}});
    enemies_b_.emplace_back(std::vector<PatrolPoint>{{7, 7}, { 11, 7 }, { 5, 7 }});
    enemies_b_.emplace_back(std::vector<PatrolPoint>{{4, 9}, { 4, 7 }, { 6, 7 }, { 6, 9 }});
}

void Game::StartTutorialChaserScene()
{
    portals_.clear();

    if (current_enemies_ != nullptr) {
        current_enemies_->clear();
    }

    is_alt_world_ = false;
    map_.UseTutorialMapC();

    ring_.SetNextTilePos(
        TutorialConst::kChaserReachRingX,
        TutorialConst::kChaserReachRingY);
    ring_.RelocateFar(player_, map_);

    chasers_.clear();
    chasers_.emplace_back(18, 7);
}

void Game::AdvanceTutorialAfterChaserReachedRing()
{
    ring_.SetNextTilePos(15, 5);
    ring_.RelocateFar(player_, map_);
}

void Game::SetupTutorialCorneredScene()
{
    ring_.SetNextTilePos(11, 11);
    ring_.RelocateFar(player_, map_);
    ring_.ClearNextPos();

    items_.clear();
    items_.emplace_back(
        TutorialConst::kItemPointAX,
        TutorialConst::kItemPointAY,
        ItemType::SLOW_ENEMY);
    items_.emplace_back(
        TutorialConst::kItemPointBX,
        TutorialConst::kItemPointBY,
        ItemType::SLOW_ENEMY);
}

void Game::ResetAfterTutorialCollapse()
{
    ResetGame();
    current_alphabet_ = GameConst::kAlphabetStart;
    prev_alphabet_ = '\0';

    map_.UseMapA();
    is_alt_world_ = false;

    player_.SetTilePos(
        TutorialConst::kResetPlayerX,
        TutorialConst::kResetPlayerY);

    enemies_a_.clear();
    enemies_b_.clear();
    chasers_.clear();
    portals_.clear();
    items_.clear();

    ring_.RelocateFar(player_, map_);
}