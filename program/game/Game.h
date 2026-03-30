#pragma once

#include <vector>

#include "GameConstants.h"
#include "Map.h"
#include "Player.h"
#include "Ring.h"
#include "PatrolEnemy.h"
#include "RingChaserEnemy.h"
#include "Portal.h"
#include "Item.h"
#include "TutorialManager.h"
#include "SignalManager.h"
#include "StageGenerator.h"

// ===== シーン種別 =====

enum class SceneType
{
    TITLE,
    TUTORIAL,
    GUIDE,
    COUNTDOWN,
    PLAY,
    RESUME_MENU,
    CLEAR
};

// ===== セーブデータ =====

struct GameState
{
    int playerTileX = 0;
    int playerTileY = 0;

    int ringTileX = 0;
    int ringTileY = 0;

    bool isAltWorld = false;

    char currentAlphabet = 'A';
    char prevAlphabet = '\0';

    double clearTime = 0.0;

    std::vector<PatrolEnemy> enemiesA;
    std::vector<PatrolEnemy> enemiesB;
};

// ===== タイル座標 =====

struct TilePos
{
    int x = 0;
    int y = 0;
};

// ===== ゲーム本体 =====

class Game
{
public:
    // =========================================================
    //  基本
    // =========================================================

    Game();

    void Update();
    void Draw();

    bool IsKeyTrigger(int keyCode) const;

    // =========================================================
    //  初期化・開始
    // =========================================================

    void ResetGame();
    void StartCountDown();
    void StartPlay();

    void SaveGameState();
    void LoadGameState();

    // =========================================================
    //  ステージ生成・マップ
    // =========================================================

    void SetupGeneratedStage();
    void ApplyCurrentGeneratedMap();

    bool IsGeneratedStageWalkable(
        const GeneratedStageData& stage,
        int tile_x,
        int tile_y) const;

    bool CanSwitchCurrentRingPosBetweenGeneratedStages() const;

    Map BuildGeneratedStageMap(
        const GeneratedStageData& stage,
        bool is_alt_world) const;

    // =========================================================
    //  シーン更新（GameSceneUpdate.cpp）
    // =========================================================

    void UpdateTitle();
    void UpdateTutorial();
    void UpdateGuide();
    void UpdateCountDown();
    void UpdatePlay();
    void UpdateResumeMenu();
    void UpdateClear();

    // =========================================================
    //  シーン描画（GameSceneDraw.cpp）
    // =========================================================

    void DrawTitle() const;
    void DrawTutorial();
    void DrawGuide() const;
    void DrawCountDown() const;
    void DrawPlay();
    void DrawResume() const;
    void DrawClear();

    // =========================================================
    //  ゲームロジック（GameSystem.cpp）
    // =========================================================

    void UpdateAlphabet();
    void DownAlphabet();
    void CheckSpawnPatrolEnemy();
    void ToggleWorld();

    void AddPortals();
    void AddChaseEnemies();
    void AddItem(ItemType type);

    void CheckPortal();
    void CheckItem();

    bool HasItem(ItemType type) const;
    int CountItems(ItemType type) const;

    void ShowSpawnMessage(const char* message);
    void UpdateSpawnMessage();
    void DrawSpawnMessage() const;

    // =========================================================
    //  配置・探索補助
    // =========================================================

    bool IsOccupiedTile(int tile_x, int tile_y) const;
    bool CanPlaceObjectTile(int tile_x, int tile_y) const;
    bool IsWideCorridorTile(int tile_x, int tile_y) const;

    int GetManhattanDistance(int x1, int y1, int x2, int y2) const;

    TilePos FindRandomFreeTile() const;
    TilePos FindRandomWideTile(
        int min_distance_from_first,
        const TilePos* first_tile = nullptr) const;

    // =========================================================
    //  リング移動
    // =========================================================

    TilePos FindRingRelocationTile(
        int from_tile_x,
        int from_tile_y,
        int min_distance,
        int max_distance) const;

    void RelocateRingInRangeFromTile(
        int from_tile_x,
        int from_tile_y,
        int min_distance,
        int max_distance);

    void RelocateRingFromPlayerInRange(int min_distance, int max_distance);

    // =========================================================
    //  チュートリアル（GameTutorial.cpp）
    // =========================================================

    void SetupTutorialStage();

    SignalManager& GetSignalManager() { return signal_manager_; }
    const SignalManager& GetSignalManager() const { return signal_manager_; }

    bool IsPlayerOnTile(int tile_x, int tile_y) const;
    bool IsAnyChaserOnTile(int tile_x, int tile_y) const;

    char GetCurrentAlphabet() const;
    char GetPreviousAlphabet() const;

    void EnterFirstRingScene();
    void StartTutorialWorldShiftScene();
    void StartTutorialChaserScene();
    void AdvanceTutorialAfterChaserReachedRing();
    void SetupTutorialCorneredScene();
    void ResetAfterTutorialCollapse();

private:
    // =========================================================
    //  管理オブジェクト
    // =========================================================

    TutorialManager tutorial_manager_;
    DialogueBox dialogue_;
    SignalManager signal_manager_;

    // =========================================================
    //  状態管理
    // =========================================================

    SceneType scene_ = SceneType::TITLE;

    bool is_first_launch_ = true;
    bool has_saved_data_ = false;
    bool is_clear_ = false;
    bool is_alt_world_ = false;
    bool has_generated_stage_pair_ = false;

    GameState saved_state_;

    char current_alphabet_ = GameConst::kInitialAlphabet;
    char prev_alphabet_ = GameConst::kNoAlphabet;

    double clear_time_ = 0.0;

    // =========================================================
    //  ゲームオブジェクト
    // =========================================================

    Map map_;
    Player player_;
    Ring ring_;

    GeneratedStageData generated_stage_a_;
    GeneratedStageData generated_stage_b_;

    std::vector<PatrolEnemy> enemies_a_;
    std::vector<PatrolEnemy> enemies_b_;
    std::vector<PatrolEnemy>* current_enemies_ = nullptr;

    std::vector<RingChaserEnemy> chasers_;
    std::vector<Portal> portals_;
    std::vector<Item> items_;

    int alphabet_boost_count_ = 0;

    // =========================================================
    //  ワープ
    // =========================================================

    bool is_warping_ = false;
    bool player_on_portal_ = false;
    int portal_cooldown_ = 0;
    int warp_from_index_ = GameConst::kInvalidIndex;
    long long warp_start_time_ = 0;

    static const int kWarpWait = GameConst::kWaitMicroseconds;

    // =========================================================
    //  敵移動
    // =========================================================

    int enemy_move_delay_ = 1;
    int next_enemy_move_delay_ = 1;
    int player_move_count_ = 0;
    int delay_item_timer_ = 0;

    // =========================================================
    //  タイマー
    // =========================================================

    long long start_time_ = 0;
    long long count_start_time_ = 0;
    int count_ = 0;

    // =========================================================
    //   入力
    // =========================================================

    char key_[GameConst::kKeyBufferSize] = {};
    char prev_key_[GameConst::kKeyBufferSize] = {};

    // =========================================================
    //   距離マップ
    // =========================================================

    int dist_[Map::H][Map::W] = {};

    // =========================================================
    //    UI
    // =========================================================

    int title_frame_ = 0;
    int tutorial_frame_ = 0;
    int guide_frame_ = 0;

    bool is_skip_popup_ = false;
    int skip_index_ = 0;
    int resume_index_ = 0;

    bool isExitPopup_ = false;
    int exitCursor_ = 0;

    char spawn_message_[128] = "";
    int spawn_message_timer_ = 0;

    // =========================================================
    //   フォント
    // =========================================================

    int logo_font_ = GameConst::kInvalidFontHandle;
    int press_font_ = GameConst::kInvalidFontHandle;
    int sub_logo_font_ = GameConst::kInvalidFontHandle;

    int tutorial_big_font_ = -1;
    int skip_font_ = -1;

    int guide_title_font_ = -1;
    int guide_font_ = -1;
    int guide_big_font_ = -1;

    int count_font_ = -1;
};