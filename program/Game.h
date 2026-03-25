#pragma once

#include <vector>

#include "Map.h"
#include "Player.h"
#include "Ring.h"
#include "PatrolEnemy.h"
#include "RingChaserEnemy.h"
#include "Portal.h"
#include "Item.h"
#include "TutorialManager.h"
#include "SignalManager.h"

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

// ===== ゲーム本体 =====

class Game
{
public:
    Game();

    // ===== 更新・描画 =====

    void Update();
    void Draw();

    // ===== 入力 =====

    bool IsKeyTrigger(int keyCode) const;

    // ===== 初期化・開始 =====

    void ResetGame();
    void SetupTutorialStage();
    void StartCountDown();
    void StartPlay();
    void SaveGameState();
    void LoadGameState();

    // ===== TutorialManager 用の公開関数 =====
    SignalManager& GetSignalManager() { return signal_manager_; }
    const SignalManager& GetSignalManager() const { return signal_manager_; }

    bool IsPlayerOnTile(int tile_x, int tile_y) const;
    bool IsAnyChaserOnTile(int tile_x, int tile_y) const;
    char GetCurrentAlphabet() const;
    char GetPreviousAlphabet() const;

    void StartTutorialWorldShiftScene();
    void StartTutorialChaserScene();
    void AdvanceTutorialAfterChaserReachedRing();
    void SetupTutorialCorneredScene();
    void ResetAfterTutorialCollapse();

private:

    // ===== シーン更新 =====

    void UpdateTitle();
    void UpdateTutorial();
    void UpdateGuide();
    void UpdateCountDown();
    void UpdatePlay();
    void UpdateResumeMenu();
    void UpdateClear();

    // ===== シーン描画 =====

    void DrawTitle() const;
    void DrawTutorial();
    void DrawGuide() const;
    void DrawCountDown() const;
    void DrawPlay();
    void DrawResume() const;
    void DrawClear();

    // ===== ゲーム進行補助 =====

    void UpdateAlphabet();
    void DownAlphabet();
    void CheckSpawnPatrolEnemy();
    void ToggleWorld();

    // ===== 配置・判定 =====

    void AddPortals();
    void AddChaseEnemies();
    void CheckPortal();
    void CheckItem();

    bool HasItem(ItemType type) const;
    int CountItems(ItemType type) const;

private:
    // ===== 管理オブジェクト =====

    TutorialManager tutorial_manager_;
    DialogueBox dialogue_;
    SignalManager signal_manager_;

    // ===== シーン・進行状態 =====

    SceneType scene_ = SceneType::TITLE;

    bool is_first_launch_ = true;
    bool has_saved_data_ = false;
    bool is_clear_ = false;
    bool is_alt_world_ = false;

    GameState saved_state_;

    char current_alphabet_ = GameConst::kInitialAlphabet;
    char prev_alphabet_ = GameConst::kNoAlphabet;

    double clear_time_ = 0.0;

    // ===== ゲームオブジェクト =====

    Map map_;
    Player player_;
    Ring ring_;

    // ===== 敵・ギミック・アイテム =====

    std::vector<PatrolEnemy> enemies_a_;
    std::vector<PatrolEnemy> enemies_b_;
    std::vector<PatrolEnemy>* current_enemies_ = nullptr;

    std::vector<RingChaserEnemy> chasers_;
    std::vector<Portal> portals_;
    std::vector<Item> items_;

    int alphabet_boost_count_ = 0;

    // ===== ワープ関連 =====

    bool is_warping_ = false;
    bool player_on_portal_ = false;
    int portal_cooldown_ = 0;
    int warp_from_index_ = GameConst::kInvalidIndex;
    long long warp_start_time_ = 0;

    static const int kWarpWait = GameConst::kWaitMicroseconds;

    // ===== 敵移動関連 =====

    int enemy_move_delay_ = 1;
    int next_enemy_move_delay_ = 1;
    int player_move_count_ = 0;
    int delay_item_timer_ = 0;

    // ===== タイマー =====

    long long start_time_ = 0;
    long long count_start_time_ = 0;
    int count_ = 0;

    // ===== 入力 =====

    char key_[GameConst::kKeyBufferSize] = {};
    char prev_key_[GameConst::kKeyBufferSize] = {};

    // ===== 距離マップ =====

    int dist_[Map::H][Map::W] = {};

    // ===== UI 状態 =====

    int title_frame_ = 0;
    int tutorial_frame_ = 0;
    int guide_frame_ = 0;

    bool is_skip_popup_ = false;
    int skip_index_ = 0;
    int resume_index_ = 0;

    // ===== フォント =====

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