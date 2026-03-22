#include <cstdio>
#include <queue>

#include "DxLib.h"

#include "Game.h"
#include "SoundID.h"
#include "SoundManager.h"

namespace {

    //==================================================
    // 敵の巡回ルート
    //==================================================

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

} // namespace


// リングから各タイルまでの距離マップを作る
void BuildDistanceMapTile(
    int goal_tile_x,
    int goal_tile_y,
    const Map& map,
    int dist[Map::H][Map::W]);

// 指定したキーが今フレームで押された瞬間かどうか
bool Game::IsKeyTrigger(int keyCode) const {
    return key[keyCode] == 1 && prevKey[keyCode] == 0;
}


// ===== ゲーム =====

// コンストラクタ：ゲーム全体の初期化
Game::Game()
    : map()
    , player(map)
    , ring() {
    logoFont = CreateFontToHandle("Yu Gothic UI", 72, 6);
    pressFont = CreateFontToHandle("Yu Gothic UI", 28, 3);
    subLogoFont = CreateFontToHandle(
        "Yu Gothic UI", 35, 6, DX_FONTTYPE_ANTIALIASING_EDGE);

    tutorialBigFont = CreateFontToHandle(
        "Yu Gothic UI", 36, 6, DX_FONTTYPE_ANTIALIASING_EDGE);

    skipFont = CreateFontToHandle(
        "Yu Gothic UI",17, DX_FONTTYPE_ANTIALIASING_EDGE);

    guideTitleFont = CreateFontToHandle(
        "Yu Gothic UI", 48, 6, DX_FONTTYPE_ANTIALIASING_EDGE);

    guideFont = CreateFontToHandle(
        "Yu Gothic UI", 28, 3, DX_FONTTYPE_ANTIALIASING_EDGE);

    guideBigFont = CreateFontToHandle(
        "Yu Gothic UI", 36, 6, DX_FONTTYPE_ANTIALIASING_EDGE);

    countFont = CreateFontToHandle(
        "Yu Gothic UI", 80, 6, DX_FONTTYPE_ANTIALIASING_EDGE);

    ResetGame();

    scene = SceneType::TITLE;
}

// ゲーム状態の初期化
void Game::ResetGame() {
    ring = Ring();
    map = Map();
    player = Player(map);

    ring.RelocateFar(player, map);

    currentAlphabet = 'A';
    prevAlphabet = '\0';

    enemiesA.clear();
    enemiesB.clear();
    chasers.clear();
    currentEnemies = &enemiesA;

    portals.clear();
    items.clear();
    enemyMoveDelay = 1;
    nextEnemyMoveDelay = 1;
    delayItemTimer = 0;

    isAltWorld = false;
    map.SetAltWorld(false);

    isClear = false;
    clearTime = 0.0;
}

// チュートリアルステージの初期配置
void Game::SetupTutorialStage() {

    map.UseTutorialMapA();
    map.SetAltWorld(false);
    isAltWorld = false;

    // ===== プレイヤー初期位置 =====
    player.SetTilePos(9, 7);

    // ===== リング初期位置 =====
    ring.SetTilePos(9, 1);

    // ===== 次のリング移動先 =====
    ring.SetNextTilePos(18, 7);

    // ===== ワープポータル =====
    portals.clear();
    portals.emplace_back(1, 1, 1);
    portals.emplace_back(18, 1, 0);

    // ===== チュートリアル開始時は敵とアイテムを消しておく =====
    chasers.clear();
    items.clear();
}

// ゲーム状態の保存
void Game::SaveGameState() {
    savedState.playerTileX = player.GetTilePosX();
    savedState.playerTileY = player.GetTilePosY();

    savedState.ringTileX = ring.GetTilePosX();
    savedState.ringTileY = ring.GetTilePosY();

    savedState.isAltWorld = isAltWorld;

    savedState.enemiesA = enemiesA;
    savedState.enemiesB = enemiesB;

    savedState.currentAlphabet = currentAlphabet;
    savedState.prevAlphabet = prevAlphabet;
    savedState.clearTime = clearTime;

    hasSavedData = true;
}

// ゲーム状態の読み込み
void Game::LoadGameState() {
    player.SetTilePos(savedState.playerTileX, savedState.playerTileY);
    ring.SetTilePos(savedState.ringTileX, savedState.ringTileY);

    isAltWorld = savedState.isAltWorld;
    map.SetAltWorld(isAltWorld);

    enemiesA = savedState.enemiesA;
    enemiesB = savedState.enemiesB;

    if (isAltWorld) {
        *currentEnemies = enemiesB;
        map.UseMapB();
    }
    else {
        *currentEnemies = enemiesA;
        map.UseMapA();
    }

    currentAlphabet = savedState.currentAlphabet;
    prevAlphabet = savedState.prevAlphabet;
    clearTime = savedState.clearTime;
}

// カウントダウン開始
void Game::StartCountDown() {
    SoundManager::StopBGM();
    count = 3;
    countStartTime = GetNowHiPerformanceCount();
    scene = SceneType::COUNTDOWN;
}

// プレイ開始
void Game::StartPlay() {
    startTime = GetNowHiPerformanceCount();
    SoundManager::PlayBGM(BGM_PLAY);
    scene = SceneType::PLAY;
}


// ===== ゲーム全体の更新処理 =====

void Game::Update()
{
	// キー入力の更新
    memcpy(prevKey, key, 256);
    GetHitKeyStateAll(key);

    switch (scene) {
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

// タイトル画面
void Game::UpdateTitle() {
    ++titleFrame;

    SoundManager::PlayBGM(BGM_TITLE);

    if (scene != SceneType::TITLE) {
        return;
    }

    if (!CheckHitKey(KEY_INPUT_RETURN)) {
        return;
    }

	// セーブデータがあるときは続きから、ないときはチュートリアルから開始
    if (hasSavedData) {
        scene = SceneType::RESUME_MENU;
    }
    else {
        tutorialManager.Init(*this);

        if (isFirstLaunch) {
            scene = SceneType::TUTORIAL;
        }
        else {
            scene = SceneType::GUIDE;
        }
    }

    SoundManager::PlaySE(SE_DECIDE);
}

// チュートリアル画面
void Game::UpdateTutorial() {

    UpdatePlay();

	// チュートリアル：スキップのポップアップ表示
    if (IsKeyTrigger(KEY_INPUT_R)) {
        isSkipPopup = true;
        skipIndex = 0;
        return;
    }

	// ポップアップ表示中の入力処理
    if (isSkipPopup) {

        if (IsKeyTrigger(KEY_INPUT_A)) {
            skipIndex = 0;
        }
        else if (IsKeyTrigger(KEY_INPUT_D)) {
            skipIndex = 1;
        }

        if (IsKeyTrigger(KEY_INPUT_RETURN)) {

            if (skipIndex == 0) {
                tutorialManager.SkipReset();
                scene = SceneType::GUIDE;
            }
            isSkipPopup = false;
        }
        return;
    }

	// チュートリアルの進行更新
    tutorialManager.Update(*this);

    if (tutorialManager.IsFinished()) {
        ResetGame();
        StartCountDown();
    }
}

// ルール説明画面
void Game::UpdateGuide() {
    ++guideFrame;

    if (IsKeyTrigger(KEY_INPUT_RETURN)) {
        SoundManager::PlaySE(SE_DECIDE);
        ResetGame();
        StartCountDown();
    }
}

// カウントダウン画面
void Game::UpdateCountDown() {
    const long long now = GetNowHiPerformanceCount();

    if (now - countStartTime < 1000000) {
        return;
    }

    --count;
    countStartTime = now;

    if (count > 0) {
        SoundManager::PlaySE(SE_COUNT);
    }
    else if (count == 0) {
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

    ring.Update(); // リングのアニメーション更新

    for (auto& item : items) {
        item.Update();
    }

    for (auto& portal : portals) {
        portal.Update();
    }

    if (portalCooldown > 0) {
        --portalCooldown;
    }

    if (!tutorialManager.IsInputLocked() && !isSkipPopup) {
        if (!isWarping) {
            player.Update(map);
        }
    }
    CheckItem();

    if (ring.CheckHit(player)) {

        if (player.IsZ()) {
            const long long now = GetNowHiPerformanceCount();
            clearTime = (now - startTime) / 1000000.0;

            SoundManager::StopBGM();
            SoundManager::PlayBGM(BGM_CLEAR);

            isClear = true;
            scene = SceneType::CLEAR;
        }
        else {

            if (scene == SceneType::PLAY) {
                ToggleWorld();
            }

            map.SetAltWorld(isAltWorld);

            UpdateAlphabet();
            CheckSpawnEnemy();

            ring.RelocateFar(player, map);
            SoundManager::PlaySE(SE_DECIDE);
        }
    }

    for (auto& enemy : *currentEnemies) {
        if (ring.CheckHit(enemy)) {
            ring.RelocateFar(enemy, map);
            SoundManager::PlaySE(SE_DECIDE);
        }
    }

    for (auto& chaser : chasers) {
        if (ring.CheckHit(chaser)) {
            ring.RelocateFar(chaser, map);
            SoundManager::PlaySE(SE_DECIDE);
        }
    }

    CheckPortal();

    if (isWarping) {
        const long long now = GetNowHiPerformanceCount();

        if (now - warpStartTime >= WARP_WAIT) {
            const int pair = portals[warpFromIndex].GetPairIndex();

            player.SetTilePos(
                portals[pair].GetTilePosX(),
                portals[pair].GetTilePosY());

            isWarping = false;
            portalCooldown = 5;
        }
    }

    if (scene == SceneType::PLAY && IsKeyTrigger(KEY_INPUT_ESCAPE)) {
        SaveGameState();
        scene = SceneType::TITLE;
    }
}

// ゲーム再開画面
void Game::UpdateResumeMenu() {

    if (IsKeyTrigger(KEY_INPUT_W)) {
        resumeIndex = 0;
    }
    if (IsKeyTrigger(KEY_INPUT_S)) {
        resumeIndex = 1;
    }

    if (!IsKeyTrigger(KEY_INPUT_RETURN)) {
        return;
    }

    if (resumeIndex == 0) {
        LoadGameState();
        scene = SceneType::PLAY;
        SoundManager::StopBGM();
        SoundManager::PlayBGM(BGM_PLAY);
    }
    else {
        hasSavedData = false;
        ResetGame();
        scene = SceneType::GUIDE;
    }
}

// ゲームクリア画面
void Game::UpdateClear() {
    if (CheckHitKey(KEY_INPUT_B)) {
        SoundManager::PlaySE(SE_DECIDE);
        scene = SceneType::TITLE;
    }

    if (CheckHitKey(KEY_INPUT_RETURN)) {
        SoundManager::PlaySE(SE_DECIDE);
        ResetGame();
        StartCountDown();
    }
}


// ===== ギミックに関する処理 =====

// 追跡敵のランダム生成
void Game::AddChaseEnemies() {

    int pos_x = 0;
    int pos_y = 0;

    do {
        pos_x = GetRand(Map::W - 1);
        pos_y = GetRand(Map::H - 1);
    } while (!map.IsWalkableTile(pos_x, pos_y));

    chasers.emplace_back(pos_x, pos_y);
}

// ポータルの追加配置
void Game::AddPortals() {

    // 通路上かつ重ならない位置に 2 つのポータルを置く
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;

    do {
        x1 = GetRand(Map::W - 1);
        y1 = GetRand(Map::H - 1);
    } while (!map.IsWalkableTile(x1, y1));

    do {
        x2 = GetRand(Map::W - 1);
        y2 = GetRand(Map::H - 1);
    } while (!map.IsWalkableTile(x2, y2) || (x1 == x2 && y1 == y2));

    portals.emplace_back(x1, y1, 1);
    portals.emplace_back(x2, y2, 0);
}

//ポータルでワープ可能か
void Game::CheckPortal() {
    if (isWarping) {
        return;
    }

    bool hit_now = false;

    for (int i = 0; i < static_cast<int>(portals.size()); ++i) {

        if (portals[i].CheckHit(player)) {

            hit_now = true;

            // 前フレームでは乗っておらず、今フレームで初めて乗ったときだけワープ開始
            if (!playerOnPortal)
            {
                isWarping = true;
                warpFromIndex = i;
                warpStartTime = GetNowHiPerformanceCount();
                SoundManager::PlaySE(SE_PORTAL);
            }
            break;
        }
    }
    playerOnPortal = hit_now;
}

// アイテムの取得判定と効果の適用
void Game::CheckItem() {

    for (int i = 0; i < static_cast<int>(items.size()); ++i)
    {
        if (!items[i].CheckHit(player)) continue;

        if (items[i].GetType() == ItemType::SLOW_ENEMY){

            // このターンではなく、次の敵行動から反映する
            nextEnemyMoveDelay ++;
            delayItemTimer = 120;
            SoundManager::PlaySE(SE_ITEM_ACCEL);
        }
        else if (items[i].GetType() == ItemType::BOOST_ALPHABET){

            alphabetBoostCount++; // 次のリング取得時の進みを加速
            SoundManager::PlaySE(SE_ITEM_SKIP);
        }

        items.erase(items.begin() + i);
        break;
    }
}

// 指定した種類のアイテムがあるか
bool Game::HasItem(ItemType type) const
{
    for (const auto& item : items) {
        if (item.GetType() == type) return true;
    }
    return false;
}

// 指定した種類のアイテム数を取得する
int Game::CountItems(ItemType type) const {
    int count = 0;

    for (const auto& item : items) {

        if (item.GetType() == type) {
            count++;
        }
    }
    return count;
}

// アルファベットを進める（リングを取ったとき）
void Game::UpdateAlphabet() {

    prevAlphabet = currentAlphabet;

    int step = 1;

    if (alphabetBoostCount > 0) {
        step = 2;
        alphabetBoostCount--;
        SoundManager::PlaySE(SE_SKIP_WORD);
    }

    for (int i = 0; i < step; i++) {

        if (currentAlphabet < 'Z') {
            player.LevelUp();
            currentAlphabet++;
        }
    }
}

// アルファベットを１つ戻す（敵に当たったとき）
void Game::DownAlphabet() {

    prevAlphabet = currentAlphabet;

    if (currentAlphabet > 'A') {
        player.LevelDown();
        currentAlphabet--;
        SoundManager::PlaySE(SE_BACK_WORD);
    }
}

// アルファベットの変化で敵やアイテムを出現させる（各１度きり）
void Game::CheckSpawnEnemy()
{
    if (scene == SceneType::PLAY)
    {
        if (currentAlphabet == prevAlphabet) return;

        bool spawned = false;

        // 巡回敵：1体目追加
        if (prevAlphabet < 'D' && currentAlphabet >= 'D') {

            if (enemiesA.size() < 1 && enemiesB.size() < 1) {
                enemiesA.emplace_back(kRouteA1);
                enemiesB.emplace_back(kRouteB1);
                spawned = true;
            }
        }

        // 巡回敵：2体目追加
        if (prevAlphabet < 'G' && currentAlphabet >= 'G') {

            if (enemiesA.size() < 2 && enemiesB.size() < 2) {
                enemiesA.emplace_back(kRouteA2);
                enemiesB.emplace_back(kRouteB2);
                AddPortals();
                spawned = true;
            }
        }

        // 追跡敵：1体目と加速アイテム追加（取得すれば効果永続）
        if (prevAlphabet < 'J' && currentAlphabet >= 'J') {

            if (chasers.size() < 1) {
                AddChaseEnemies();
                spawned = true;
            }

            if (!HasItem(ItemType::SLOW_ENEMY) && enemyMoveDelay < 2) {
                items.emplace_back(Item::CreateRandom(map, ItemType::SLOW_ENEMY));
                spawned = true;
            }
        }

        // 促進アイテム追加（リングを取得すると効果リセットして、新たにアイテム出現）
        if (currentAlphabet >= 'M') {

            if (!HasItem(ItemType::BOOST_ALPHABET)) {
                items.emplace_back(Item::CreateRandom(map, ItemType::BOOST_ALPHABET));
                spawned = true;
            }
        }

        // 追跡敵：2体目と加速アイテム追加
        if (prevAlphabet < 'P' && currentAlphabet >= 'P') {

            if (chasers.size() < 2) {
                AddChaseEnemies();
                spawned = true;
            }

            if (enemyMoveDelay < 2) // 1回も取っていない
            {
                if (CountItems(ItemType::SLOW_ENEMY) < 2) {
                    items.emplace_back(Item::CreateRandom(map, ItemType::SLOW_ENEMY));
                    spawned = true;
                }
            }
            else if (enemyMoveDelay == 3) // 1回だけ取った
            {
                if (!HasItem(ItemType::SLOW_ENEMY)) {
                    items.emplace_back(Item::CreateRandom(map, ItemType::SLOW_ENEMY));
                    spawned = true;
                }
            }
        }

        // ワープポータル追加
        if (prevAlphabet < 'S' && currentAlphabet >= 'S') {
            AddPortals();
            spawned = true;
        }

        // 追跡敵：3体目追加
        if (prevAlphabet < 'V' && currentAlphabet >= 'V') {

            if (chasers.size() < 3) {
                AddChaseEnemies();
                spawned = true;
            }
        }

        if (spawned) {
            SoundManager::PlaySE(SE_ENEMY_SPAWN);
        }
    }
}

// 世界を切り替える（リングが両方の世界で通行可能な位置にあるときだけ）
void Game::ToggleWorld() {

    if (!map.CanSwitchRingPos(ring.GetTilePosX(), ring.GetTilePosY())) {
        return;
    }

    isAltWorld = !isAltWorld;
    map.SetAltWorld(isAltWorld);

    if (isAltWorld) {
        map.UseMapB();
        currentEnemies = &enemiesB;
    }
    else {
        map.UseMapA();
        currentEnemies = &enemiesA;
    }

    portals.clear();
    items.clear();
    chasers.clear();

    //ステージ切り替え時のギミックリセット
    if (currentAlphabet >= 'G') {
        AddPortals();
    }

    if (currentAlphabet >= 'J') {
        AddChaseEnemies();
        if (enemyMoveDelay == 1) 
            items.emplace_back(Item::CreateRandom(map, ItemType::SLOW_ENEMY));
    }

    if (currentAlphabet >= 'M') {
        items.emplace_back(Item::CreateRandom(map, ItemType::BOOST_ALPHABET));
    }

    if (currentAlphabet >= 'P') {
        AddChaseEnemies();

        if (enemyMoveDelay == 2) {
            items.emplace_back(Item::CreateRandom(map, ItemType::SLOW_ENEMY));
            items.emplace_back(Item::CreateRandom(map, ItemType::SLOW_ENEMY));
        }
    }

    if (currentAlphabet >= 'S') {
        AddPortals();
    }

    if (currentAlphabet >= 'V') {
        AddChaseEnemies();
    }

    SoundManager::PlaySE(SE_PORTAL);
}


// ===== ゲーム全体の描画処理 =====

void Game::Draw() {
    switch (scene) {
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

// タイトル画面
void Game::DrawTitle() const {

    DrawBox(0, 0, 640, 480, GetColor(40, 40, 50), TRUE);

    for (int y = 0; y < 480; y += 40)
    {
        for (int x = 0; x < 640; x += 80)
        {
            DrawBox(x, y, x + 78, y + 38, GetColor(60, 60, 70), FALSE);
        }
    }

    for (int i = 0; i < 20; i++)
    {
        int x = (titleFrame * 2 + i * 60) % 640;
        int y = (i * 40 + titleFrame) % 480;
        int gray = 40 + static_cast<int>(60 * (sin((titleFrame + i * 10) * 0.05) + 1) * 0.5);

        DrawCircle(x, y, 25, GetColor(gray, gray, gray), TRUE);
    }

    const char* logo = "ChangeRing O";
    int drawX = 100;
    int baseY = 170;

    for (int i = 0; logo[i] != '\0'; i++)
    {
        char c[2] = { logo[i], '\0' };
        int drawY = baseY + static_cast<int>(sin(titleFrame * 0.08 + i * 0.4) * 8);
        int width = GetDrawStringWidthToHandle(c, 1, logoFont);

        DrawStringToHandle(drawX + 6, drawY + 6, c, GetColor(120, 0, 120), logoFont);

        for (int ox = -3; ox <= 3; ox++)
        {
            for (int oy = -3; oy <= 3; oy++)
            {
                if (ox * ox + oy * oy <= 9)
                {
                    DrawStringToHandle(
                        drawX + ox, drawY + oy, c,
                        GetColor(255, 255, 255), logoFont);
                }
            }
        }

        double t = (titleFrame + i * 10) * 0.05;
        double wave = (sin(t) + 1) * 0.5;

        int r = static_cast<int>(200 * wave);
        int g = 200;
        int b = static_cast<int>(200 * (1.0 - wave));

        DrawStringToHandle(drawX, drawY, c, GetColor(r, g, b), logoFont);
        drawX += width;
    }

    if ((titleFrame / 30) % 2 == 0)
    {
        DrawStringToHandle(
            250, 350,
            "Press Enter!",
            GetColor(255, 255, 255),
            pressFont);
    }
}

// チュートリアル画面
void Game::DrawTutorial()
{
    DrawPlay();
    tutorialManager.Draw();

    // チュートリアルのスキップポップアップ 
    if (!isSkipPopup) {

        DrawStringToHandle(
            540, 10,
            "[R]でスキップ",
            GetColor(255, 255, 255),
            skipFont);
    }

    if (isSkipPopup)
    {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
        DrawBox(0, 0, 640, 480, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

        DrawBox(150, 150, 490, 300, GetColor(30, 30, 50), TRUE);
        DrawBox(150, 150, 490, 300, GetColor(200, 200, 255), FALSE);

        DrawString(180, 180, "チュートリアルをスキップしますか？", GetColor(255, 255, 255));

        const char* yes = "Yes";
        const char* no = "No";

        DrawString(240, 230, yes, skipIndex == 0 ? GetColor(255, 255, 0) : GetColor(200, 200, 200));
        DrawString(360, 230, no, skipIndex == 1 ? GetColor(255, 255, 0) : GetColor(200, 200, 200));
    }
}

// ルール説明画面
void Game::DrawGuide() const {

    DrawBox(0, 0, 640, 480, GetColor(15, 15, 25), TRUE);

    const char* title = "- GAME GUIDE -";
    int width = GetDrawStringWidthToHandle(title, -1, guideTitleFont);
    int x = 320 - width / 2;

    DrawStringToHandle(x + 10, 110, title, GetColor(0, 0, 0), guideTitleFont);
    DrawStringToHandle(x, 100, title, GetColor(255, 220, 80), guideTitleFont);

    DrawString(80, 190,
        "① リング 〇に入ると、アルファベット『A』が進む（A→B→…）",
        GetColor(255, 255, 255));

    DrawString(80, 230,
        "② 敵△と重なると、アルファベットが１つ戻る（C→B→A）",
        GetColor(255, 100, 255));

    DrawString(80, 270,
        "③ アルファベット『Z』で〇に入るとゲームクリア！",
        GetColor(255, 255, 255));

    DrawString(80, 310,
        "④ 〇が〈他のステージ〉と重なる位置にあれば、ステージも変わる",
        GetColor(255, 255, 100));

    if ((guideFrame / 30) % 2 == 0)
    {
        DrawStringToHandle(
            150, 380,
            "Press Enter to Start",
            GetColor(190, 200, 255),
            guideBigFont);
    }
}

// カウントダウン画面
void Game::DrawCountDown() const {

    if (count > 0)
    {
        char buf[8];
        sprintf_s(buf, "%d", count);

        int width = GetDrawStringWidthToHandle(buf, 1, countFont);
        int x = 320 - width / 2;
        int y = 180;

        DrawStringToHandle(x + 6, y + 6, buf, GetColor(0, 0, 0), countFont);
        DrawStringToHandle(x, y, buf, GetColor(255, 255, 255), countFont);
    }
    else
    {
        const char* text = "Start!";
        int width = GetDrawStringWidthToHandle(text, 6, countFont);
        int x = 320 - width / 2;
        int y = 180;

        DrawStringToHandle(x + 6, y + 6, text, GetColor(0, 0, 0), countFont);
        DrawStringToHandle(x, y, text, GetColor(255, 255, 0), countFont);
    }
}

// プレイ画面
void Game::DrawPlay() {

    if (isAltWorld)
    {
        DrawBox(0, 0, 640, 480, GetColor(10, 10, 25), TRUE);
    }
    else
    {
        DrawBox(0, 0, 640, 480, GetColor(255, 40, 50), TRUE);
    }

    map.Draw();

    for (auto& portal : portals)
    {
        portal.Draw();
    }

    for (auto& item : items)
    {
        item.Draw();
    }

    ring.Draw();
    player.Draw();

    for (const auto& chaser : chasers)
    {
        chaser.Draw();
    }

    if (player.HasDrawPositionChanged())
    {
        playerMoveCount++;
        tutorialManager.step++;

        if (playerMoveCount >= enemyMoveDelay)
        {
            playerMoveCount = 0;

            for (auto& enemy : *currentEnemies)
            {
                enemy.Update(map);

                if (enemy.CheckHit(player))
                {
                    DownAlphabet();
                }
            }

            BuildDistanceMapTile(
                ring.GetTilePosX(),
                ring.GetTilePosY(),
                map,
                dist);

            for (auto& chaser : chasers)
            {
                chaser.Update(dist, map);

                if (chaser.CheckHit(player))
                {
                    DownAlphabet();
                }
            }

            // 敵の移動が終わったあとで次の遅延設定を反映
            enemyMoveDelay = nextEnemyMoveDelay;
        }
    }

    for (auto& enemy : *currentEnemies)
    {
        enemy.Draw();
    }

    player.Draw();
    player.CommitDrawPosition();
}

// ゲーム再開画面
void Game::DrawResume() const {

    for (int y = 0; y < 480; y++)
    {
        DrawLine(0, y, 640, y, GetColor(10, 10, 10));
    }

    DrawBox(120, 170, 520, 350, GetColor(30, 30, 50), TRUE);
    DrawBox(120, 170, 520, 350, GetColor(80, 80, 150), FALSE);

    DrawStringToHandle(
        100, 80,
        "- ACCESS POINT -",
        GetColor(250, 220, 0),
        guideTitleFont);

    for (int i = 0; i < 2; i++)
    {
        int y = 220 + i * 60;
        const char* text = (i == 0) ? "Continue" : "New Game";

        DrawStringToHandle(
            250, y - 5,
            text,
            resumeIndex == i
            ? GetColor(255, 255, 255)
            : GetColor(160, 160, 200),
            guideFont);
    }
}

// ゲームクリア画面
void Game::DrawClear() {

    player.Draw();

    DrawString(220, 160, "GAME CLEAR!", GetColor(255, 255, 0));

    char buf[64];
    sprintf_s(buf, "CLEAR TIME : %.2f sec", clearTime);
    DrawString(180, 210, buf, GetColor(255, 255, 255));

    DrawString(140, 260, "Enter : Retry   B : Title", GetColor(200, 200, 200));
}


// BFS でリングから各タイルへの距離を計算する
void BuildDistanceMapTile(int goal_tile_x, int goal_tile_y,
    const Map& map, int dist[Map::H][Map::W])
{
    for (int y = 0; y < Map::H; y++)
    {
        for (int x = 0; x < Map::W; x++)
        {
            dist[y][x] = -1;
        }
    }

    if (goal_tile_x < 0 || goal_tile_y < 0 ||
        goal_tile_x >= Map::W || goal_tile_y >= Map::H)
    {
        return;
    }

    if (!map.IsWalkableTile(goal_tile_x, goal_tile_y))
    {
        return;
    }

    std::queue<std::pair<int, int>> q;
    dist[goal_tile_y][goal_tile_x] = 0;
    q.push({ goal_tile_x, goal_tile_y });

    const int dx[4] = { 1, -1, 0, 0 };
    const int dy[4] = { 0, 0, 1, -1 };

    while (!q.empty())
    {
        auto [x, y] = q.front();
        q.pop();

        for (int i = 0; i < 4; i++)
        {
            int nx = x + dx[i];
            int ny = y + dy[i];

            if (nx < 0 || ny < 0 || nx >= Map::W || ny >= Map::H) continue;

            if (!map.IsWalkableTile(nx, ny)) continue;

            if (dist[ny][nx] != -1) continue;

            dist[ny][nx] = dist[y][x] + 1;
            q.push({ nx, ny });
        }
    }
}