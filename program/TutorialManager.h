#pragma once

#include <string>
#include <vector>

#include "Constants.h"

class Game;

// チュートリアル進行の状態
enum class TutorialPhase
{
    // ===== 導入 =====
    WAKE_UP,
    TUTORIAL_SKIP,

    // ===== 最初のリング =====
    MOVE_TO_FIRST_TRIGGER,
    NOTICE_RING,
    MOVE_NEAR_FIRST_RING,
    CONSIDER_FIRST_RING,
    STEP_INTO_FIRST_RING,
    REACT_TO_FIRST_RING,

    // ===== ポータルと世界切り替え =====
    MOVE_TO_PORTAL,
    REACT_TO_PORTAL,
    MOVE_TO_WORLD_SHIFT_RING,
    REACT_TO_WORLD_SHIFT,

    // ===== 通常敵のチュートリアル =====
    MOVE_NEAR_SUSPICIOUS_ENEMY,
    WARN_ABOUT_ENEMY,
    STEP_TOWARD_ENEMY,
    REACT_TO_ENEMY_HIT,
    MOVE_TO_SAFE_RING,
    REFLECT_ON_ENEMY_TRAP,
    STEP_INTO_ESCAPE_RING,

    // ===== リング追跡敵のチュートリアル =====
    NOTICE_RING_CHASER,
    WAIT_FOR_RING_CHASER_APPROACH,
    UNDERSTAND_RING_CHASER,
    WAIT_FOR_RING_CHASER_REACH_RING,
    CONFIRM_RING_CHASER_BEHAVIOR,
    WAIT_FOR_NEXT_RING_MOVE,

    // ===== アイテムのチュートリアル =====
    FEEL_CORNERED,
    MOVE_TO_MYSTERIOUS_ITEM,
    REACT_TO_MYSTERIOUS_ITEM,
    WAIT_FOR_SPEED_EFFECT,
    UNDERSTAND_SPEED_BOOST,
    REACH_ALPHABET_E,

    // ===== 終盤演出 =====
    COLLAPSE_WARNING,
    FADE_OUT_AND_RESET,
    WAKE_AFTER_RESET,

    // ===== 終了 =====
    TUTORIAL_END
};

// セリフ表示用ダイアログボックス
// 表示文字列の保持、改行分割、描画を担当する
class DialogueBox
{
public:
    // ===== 初期化・表示制御 =====

    // フォントなど描画に必要な初期化を行う
    void Init();

    // 指定したテキストを表示する
    void Show(const char* text);

    // 表示を閉じる
    void Hide();

    // ===== 描画・状態取得 =====

    // ダイアログボックスを描画する
    void Draw();

    // セリフを表示中かどうかを返す
    bool IsShowing() const;

private:
    // テキストを描画用に複数行へ分割する
    std::vector<std::string> SplitLines(const char* text);

    // 現在表示中のテキスト
    const char* current_ = nullptr;

    // 描画に使うフォントハンドル
    int font_handle_ = FontConst::kInvalidHandle;
};

// チュートリアル全体の進行を管理するクラス
// フェーズ遷移、会話進行、演出制御、進行条件判定を担当する
class TutorialManager
{
public:
    // ===== 初期化・更新 =====

    // チュートリアル開始時の初期化
    void Init(Game& game);

    // チュートリアルの進行更新
    void Update(Game& game);

    // チュートリアル関連の UI や演出を描画する
    void Draw();

    // ===== 状態取得 =====

    // チュートリアルが終了しているかを返す
    bool IsFinished() const;

    // 会話や演出中など、プレイヤー入力を止めるべき状態かを返す
    bool IsInputLocked() const;

    // セリフを表示中かどうかを返す
    bool IsDialogueShowing() const;

    // ===== 補助判定 =====

    // プレイヤーが指定タイル上にいるかを判定する
    bool IsPlayerOn(Game& game, int tile_x, int tile_y);

    // 敵が指定タイル上にいるかを判定する
    bool IsChaserOn(Game& game, int tile_x, int tile_y);

    // ===== 特殊操作 =====

    // スキップ用の状態へ切り替える
    void SkipReset();

private:
    // ===== 進行状態 =====

    // 現在のチュートリアル進行段階
    TutorialPhase phase_ = TutorialPhase::WAKE_UP;

    // セリフ表示用ダイアログ
    DialogueBox dialogue_;

    // 会話の進行度
    int talk_ = 0;

    // 時間経過で進む演出用タイマー
    int timer_ = 0;

    // 移動シグナルを受け取った回数
    int move_signal_count_ = 0;

    // ===== フェード演出関連 =====

    // フェードの透明度
    // 0 = 透明、255 = 完全不透明
    int fade_alpha_ = 0;

    // フェードアウト中かどうか
    bool is_fading_out_ = false;

    // フェードイン中かどうか
    bool is_fading_in_ = false;

    // 暗転待機開始時刻
    long long fade_wait_start_ = 0;

    // 暗転中の待機状態かどうか
    bool is_waiting_ = false;
};