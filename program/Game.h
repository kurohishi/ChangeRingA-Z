#pragma once

#include <vector>

#include "Player.h"
#include "Ring.h"
#include "Map.h"
#include "Enemy.h"
#include "RingChaserEnemy.h"
#include "Portal.h"
#include "Item.h"
#include "TutorialManager.h"

// 現在のゲーム画面状態を表す
enum class SceneType {
    TUTORIAL,
    TITLE,
    GUIDE,
    COUNTDOWN,
    PLAY,
    RESUME_MENU,
    CLEAR
};

// ゲーム途中再開用の保存状態
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

class Game {
public:
    Game();
    void Update();
    void Draw();

    // 現在のアルファベット状態
    char currentAlphabet;
    char prevAlphabet;

private:
    // チュートリアルからゲーム内部状態へアクセスするために使用する
    friend class TutorialManager;
    TutorialManager tutorialManager;
	DialogueBox dialogue;

    bool isFirstLaunch = true;
    GameState savedState;
    bool hasSavedData = false;

    void SetupTutorialStage();

    void SaveGameState();
    void LoadGameState();
    void UpdateResumeMenu();
    void ResetGame();
    void StartCountDown();
    void StartPlay();

    void UpdateTutorial();
    void UpdateTitle();
    void UpdateGuide();
    void UpdateCountDown();
    void UpdatePlay();
    void UpdateClear();
    void UpdateAlphabet();
    void DownAlphabet();
    void CheckSpawnEnemy();

    void DrawTutorial();
    void DrawTitle() const;
    void DrawGuide() const;
    void DrawCountDown() const;
    void DrawPlay();
    void DrawResume() const;
    void DrawClear();

    void InitPortals();
    void CheckPortal();
    void AddItems(ItemType type);
    void CheckItem();

    void ToggleWorld();
    void InitRouteEnemies();
    void AddChaseEnemies();

    SceneType scene;
    Map map;
    Player player;
    Ring ring;

    std::vector<Enemy> enemiesA;
    std::vector<Enemy> enemiesB;
    std::vector<Enemy>* currentEnemies = nullptr;
    std::vector<Enemy> enemies;
    std::vector<RingChaserEnemy> chasers;
    std::vector<Portal> portals;
    std::vector<Item> items;
    int alphabetBoostCount = 0;

    int portalCooldown = 0;
    bool playerOnPortal = false;

    bool isClear = false;
    double clearTime = 0.0;

    long long startTime = 0;
    long long countStartTime = 0;
    int count = 0;

    int titleFrame = 0;
    int logoFont = -1;
    int pressFont = -1;
    int subLogoFont = -1;

    int tutorialBigFont = -1;
    int skipFont = -1;
    int tutorialFrame = 0;
    bool isSkipPopup = false;
    int skipIndex = 0;

    int guideTitleFont = -1;
    int guideFont = -1;
    int guideBigFont = -1;
    int guideFrame = 0;

    int countFont = -1;

    int dist[Map::H][Map::W];

    char key[256];
    char prevKey[256];
    bool IsKeyTrigger(int keyCode) const;

    //アイテム判定用
    bool HasItem(ItemType type) const;
    int CountItems(ItemType type) const;

    bool isWarping = false;
    int warpFromIndex = -1;
    long long warpStartTime = 0;
    static const int WARP_WAIT = 1000000;

    int resumeIndex = 0;

    // 敵の移動遅延関連
    int enemyMoveDelay = 1;
    int nextEnemyMoveDelay = 1;
    int playerMoveCount = 0;
    int delayItemTimer = 0;

    // 裏世界状態
    bool isAltWorld = false;
};