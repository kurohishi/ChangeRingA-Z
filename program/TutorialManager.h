#pragma once

#include <string>
#include <vector>

class Game;

// チュートリアル進行の状態を表す
enum class TutorialPhase {

    WAKE_UP,
    TUTORIAL_SKIP,

    MOVE_TO_FIRST_TRIGGER,
    NOTICE_RING,

    MOVE_NEAR_FIRST_RING,
    CONSIDER_FIRST_RING,
    STEP_INTO_FIRST_RING,
    REACT_TO_FIRST_RING,

    MOVE_TO_PORTAL,
    REACT_TO_PORTAL,

    MOVE_TO_WORLD_SHIFT_RING,
    REACT_TO_WORLD_SHIFT,

    MOVE_NEAR_SUSPICIOUS_ENEMY,
    WARN_ABOUT_ENEMY,
    STEP_TOWARD_ENEMY,
    REACT_TO_ENEMY_HIT,

    MOVE_TO_SAFE_RING,
    REFLECT_ON_ENEMY_TRAP,
    STEP_INTO_ESCAPE_RING,

    NOTICE_RING_CHASER,
    WAIT_FOR_RING_CHASER_APPROACH,
    UNDERSTAND_RING_CHASER,

    WAIT_FOR_RING_CHASER_REACH_RING,
    CONFIRM_RING_CHASER_BEHAVIOR,
    WAIT_FOR_NEXT_RING_MOVE,

    FEEL_CORNERED,

    MOVE_TO_MYSTERIOUS_ITEM,
    REACT_TO_MYSTERIOUS_ITEM,
    WAIT_FOR_SPEED_EFFECT,
    UNDERSTAND_SPEED_BOOST,

    REACH_ALPHABET_E,

    COLLAPSE_WARNING,
    FADE_OUT_AND_RESET,
    WAKE_AFTER_RESET,

    TUTORIAL_END
};

// セリフ表示用のダイアログボックス ※表示文字列の保持、改行分割、描画
class DialogueBox {
public:
    // フォントなど描画に必要な初期化を行う
    void Init();

    // 指定したテキストを表示する
    void Show(const char* text);

    // 表示を閉じる
    void Hide();

    // ダイアログボックスを描画する
    void Draw();

    // セリフを表示中かどうかを返す
    bool IsShowing() const;

private:

    // 現在表示中のテキスト
    const char* current_ = nullptr;

    // テキストを描画用に複数行へ分割する
    std::vector<std::string> SplitLines(const char* text);

    // 描画に使うフォントハンドル
    int font_handle_ = -1;
};

// チュートリアル全体の進行管理を行うクラス ※フェーズ遷移、会話進行、演出制御、進行条件の判定
class TutorialManager {
public:

    // チュートリアル開始時の初期化
    void Init(Game& game); 

    // チュートリアルの進行更新
    void Update(Game& game);

    // チュートリアル関連のUIや演出の描画
    void Draw();

    // チュートリアルが終了しているかを返す
    bool IsFinished() const;

	// スキップで初期化する
    void SkipReset();

    // 会話や演出中など、プレイヤー入力を止めるべき状態かを返す
    bool IsInputLocked() const;

    // プレイヤーが指定したタイル上にいるかを判定する
    bool IsPlayerOn(Game& game, int tile_x, int tile_y);

    // 敵が指定したタイル上にいるかを判定する
    bool IsEnemyOn(Game& game, int tile_x, int tile_y);

    bool IsDialogueShowing() const;

    // 現在の移動進行度
    int step = 0;

private:
    // 現在のチュートリアル進行段階
    TutorialPhase phase_ = TutorialPhase::WAKE_UP;

    // セリフ表示用ダイアログ
    DialogueBox dialogue_;

    // 会話の進行度
    int talk_ = 0;

    // 時間経過で進む演出用タイマー
    int timer_ = 0;

    // ===== フェード演出関連 =====

    // フェードの透明度0 が透明、255 が完全不透明
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