#pragma once

#include <vector>

#include "Map.h"
#include "Player.h"
#include "Ring.h"
#include "Enemy.h"
#include "RingChaserEnemy.h"
#include "Portal.h"
#include "Item.h"
#include "TutorialManager.h"

//==================================================
// 画面状態
//==================================================

enum class SceneType {
    TITLE,
    TUTORIAL,
    GUIDE,
    COUNTDOWN,
    PLAY,
    RESUME_MENU,
    CLEAR
};

//==================================================
// セーブデータ
//==================================================

struct GameState {
    int playerTileX = 0;
    int playerTileY = 0;

    int ringTileX = 0;
    int ringTileY = 0;

    bool isAltWorld = false;

    char currentAlphabet = 'A';
    char prevAlphabet = '\0';

    double clearTime = 0.0;

    std::vector<Enemy> enemiesA;
    std::vector<Enemy> enemiesB;
};

//==================================================
// ゲーム本体
//==================================================

class Game {
public:
    Game();

    void Update();
    void Draw();

private:

    // チュートリアルから内部状態へアクセスするために使用
    friend class TutorialManager;

    //==================================================
    // 初期化・開始
    //==================================================

    void ResetGame();
    void SetupTutorialStage();
    void StartCountDown();
    void StartPlay();

    void SaveGameState();
    void LoadGameState();

    //==================================================
    // 更新処理
    //==================================================

    void UpdateTitle();
    void UpdateTutorial();
    void UpdateGuide();
    void UpdateCountDown();
    void UpdatePlay();
    void UpdateResumeMenu();
    void UpdateClear();

    //==================================================
    // 描画処理
    //==================================================

    void DrawTitle() const;
    void DrawTutorial();
    void DrawGuide() const;
    void DrawCountDown() const;
    void DrawPlay();
    void DrawResume() const;
    void DrawClear();

    //==================================================
    // ゲーム進行補助
    //==================================================

    void UpdateAlphabet();
    void DownAlphabet();
    void CheckSpawnEnemy();
    void ToggleWorld();

    //==================================================
    // 配置・判定
    //==================================================

    void AddPortals();
    void AddChaseEnemies();
    void CheckPortal();
    void CheckItem();

    bool HasItem(ItemType type) const;
    int CountItems(ItemType type) const;

    //==================================================
    // 入力補助
    //==================================================

    bool IsKeyTrigger(int keyCode) const;

private:
    //==================================================
    // 管理オブジェクト
    //==================================================

    TutorialManager tutorialManager;
    DialogueBox dialogue;

    //==================================================
    // シーン・進行状態
    //==================================================

    SceneType scene = SceneType::TITLE;

    bool isFirstLaunch = true;
    bool hasSavedData = false;
    bool isClear = false;
    bool isAltWorld = false;

    GameState savedState;

    char currentAlphabet = 'A';
    char prevAlphabet = '\0';

    double clearTime = 0.0;

    //==================================================
    // ゲームオブジェクト
    //==================================================

    Map map;
    Player player;
    Ring ring;

    //==================================================
    // 敵・ギミック・アイテム
    //==================================================

    std::vector<Enemy> enemiesA;
    std::vector<Enemy> enemiesB;
    std::vector<Enemy>* currentEnemies = nullptr;

    std::vector<RingChaserEnemy> chasers;
    std::vector<Portal> portals;
    std::vector<Item> items;

    int alphabetBoostCount = 0;

    //==================================================
    // ワープ関連
    //==================================================

    bool isWarping = false;
    bool playerOnPortal = false;
    int portalCooldown = 0;
    int warpFromIndex = -1;
    long long warpStartTime = 0;
    static const int WARP_WAIT = 1000000;

    //==================================================
    // 敵移動関連
    //==================================================

    int enemyMoveDelay = 1;
    int nextEnemyMoveDelay = 1;
    int playerMoveCount = 0;
    int delayItemTimer = 0;

    //==================================================
    // タイマー
    //==================================================

    long long startTime = 0;
    long long countStartTime = 0;
    int count = 0;

    //==================================================
    // 入力
    //==================================================

    char key[256] = {};
    char prevKey[256] = {};

    //==================================================
    // 距離マップ
    //==================================================

    int dist[Map::H][Map::W] = {};

    //==================================================
    // UI状態
    //==================================================

    int titleFrame = 0;
    int tutorialFrame = 0;
    int guideFrame = 0;

    bool isSkipPopup = false;
    int skipIndex = 0;
    int resumeIndex = 0;

    //==================================================
    // フォント
    //==================================================

    int logoFont = -1;
    int pressFont = -1;
    int subLogoFont = -1;

    int tutorialBigFont = -1;
    int skipFont = -1;

    int guideTitleFont = -1;
    int guideFont = -1;
    int guideBigFont = -1;

    int countFont = -1;
};