#include "TutorialManager.h"

#include <string>

#include "GameConstants.h"
#include "DxLib.h"
#include "Game.h"
#include "GameTexts.h"
#include "Map.h"

namespace
{
    // ===== 会話進行入力 =====
    void AdvanceTalkOnEnter(Game& game, int& talk)
    {
        if (game.IsKeyTrigger(KEY_INPUT_RETURN)) {
            ++talk;
        }
    }

	// ===== 会話表示補助 =====

     // 次のフェーズに切り替える（会話の進行度をリセット/セリフを非表示にする
    void ChangePhase(
        TutorialPhase& phase,
        int& talk,
        DialogueBox& dialogue,
        TutorialPhase next_phase,
        int next_talk = 1,
        bool hide_dialogue = true)
    {
        phase = next_phase;
        talk = next_talk;

        if (hide_dialogue) {
            dialogue.Hide();
        }
    }

    template <size_t N>
    void ShowTalkLine(
        DialogueBox& dialogue,
        int talk,
        const char* const (&lines)[N])
    {
        if (talk >= 0 &&
            talk < static_cast<int>(N) &&
            lines[talk] != nullptr) {
            dialogue.Show(lines[talk]);
        }
    }

	// 会話の配列から、指定した進行度のセリフを返す
    template <size_t N>
    const char* GetTalkFromArray(const char* const (&talks)[N], int talk)
    {
        if (talk < 0 || talk >= static_cast<int>(N)) {
            return nullptr;
        }
        return talks[talk];
    }

    // 対応するセリフを表示する
    void ShowSingleTalk(DialogueBox& dialogue, const char* text)
    {
        if (text != nullptr) {
            dialogue.Show(text);
        }
    }

	// 状況に合わせてセリフを分岐する（最初の敵と何回接触したか）
    const char* GetReactToEnemyHitTalk(const Game& game, int talk)
    {
        if (game.GetPreviousAlphabet() == 'A' &&
            game.GetCurrentAlphabet() == 'A') {
            return GetTalkFromArray(
                GameText::Tutorial::ReactToEnemyHit::StayA::kTalks,
                talk);
        }

        if (game.GetPreviousAlphabet() == 'B' &&
            game.GetCurrentAlphabet() == 'A') {
            return GetTalkFromArray(
                GameText::Tutorial::ReactToEnemyHit::BtoA::kTalks,
                talk);
        }

        if (game.GetPreviousAlphabet() == 'C' &&
            game.GetCurrentAlphabet() == 'B') {
            return GetTalkFromArray(
                GameText::Tutorial::ReactToEnemyHit::CtoB::kTalks,
                talk);
        }

        return nullptr;
    }

	// 状況に合わせてセリフを分岐する（リング手前までに何回接触したか）
    const char* GetReflectOnEnemyTrapTalk(const Game& game, int talk)
    {
        if (game.GetPreviousAlphabet() == 'A' &&
            game.GetCurrentAlphabet() == 'A') {
            return GetTalkFromArray(
                GameText::Tutorial::ReflectOnEnemyTrap::StayA::kTalks,
                talk);
        }

        if (game.GetPreviousAlphabet() == 'B' &&
            game.GetCurrentAlphabet() == 'A') {
            return GetTalkFromArray(
                GameText::Tutorial::ReflectOnEnemyTrap::BtoA::kTalks,
                talk);
        }

        if (game.GetPreviousAlphabet() == 'C' &&
            game.GetCurrentAlphabet() == 'B') {
            return GetTalkFromArray(
                GameText::Tutorial::ReflectOnEnemyTrap::CtoB::kTalks,
                talk);
        }

        return nullptr;
    }
}  // namespace

// ===== DialogueBox =====

// ダイアログボックスの初期化
void DialogueBox::Init()
{
    font_handle_ = CreateFontToHandle(
        nullptr,
        FontConst::kDialogueSize,
        FontConst::kDialogueThickness);
}

// ダイアログボックスにテキストを表示する
void DialogueBox::Show(const char* text)
{
    current_ = text;
}

// ダイアログボックスを閉じる
void DialogueBox::Hide()
{
    current_ = nullptr;
}

// ダイアログボックスが表示中か
bool DialogueBox::IsShowing() const
{
    return current_ != nullptr;
}

// ダイアログボックスの描画
void DialogueBox::Draw()
{
    if (!current_) {
        return;
    }

    const int box_height =
        MapConst::kMapTile * TutorialConst::kDialogueBoxTileHeight;

    const int box_x1 = 0;
    const int box_y1 = GameConst::kScreenHeight - box_height;
    const int box_x2 = GameConst::kScreenWidth;
    const int box_y2 = GameConst::kScreenHeight;

    DrawBox(box_x1, box_y1, box_x2, box_y2, GetColor(0, 0, 0), TRUE);
    DrawBox(
        box_x1 + TutorialConst::kDialogueBorderMargin,
        box_y1 + TutorialConst::kDialogueBorderMargin,
        box_x2 - TutorialConst::kDialogueBorderMargin,
        box_y2 - TutorialConst::kDialogueBorderMargin,
        GetColor(255, 255, 255),
        FALSE);

    const int center_x = (box_x1 + box_x2) / 2;
    const int center_y = (box_y1 + box_y2) / 2;

    const int text_height = GetFontSizeToHandle(font_handle_);
    const std::vector<std::string> lines = SplitLines(current_);
    const int total_height = static_cast<int>(lines.size()) * text_height;
    const int start_y = center_y - total_height / 2;

    for (int i = 0; i < static_cast<int>(lines.size()); ++i) {
        const std::string& line = lines[i];
        const int line_width = GetDrawStringWidthToHandle(
            line.c_str(),
            static_cast<int>(line.size()),
            font_handle_);

        const int draw_x = center_x - line_width / 2;
        const int draw_y = start_y + i * text_height;

        DrawStringToHandle(
            draw_x,
            draw_y,
            line.c_str(),
            GetColor(255, 255, 255),
            font_handle_);
    }
}

// テキストを改行コードで複数行に分割する
std::vector<std::string> DialogueBox::SplitLines(const char* text)
{
    std::vector<std::string> result;
    std::string current_line;

    for (const char* p = text; *p; ++p) {
        if (*p == '\n') {
            result.push_back(current_line);
            current_line.clear();
        }
        else {
            current_line += *p;
        }
    }

    if (!current_line.empty()) {
        result.push_back(current_line);
    }

    return result;
}

// ===== チュートリアル関連  =====

// チュートリアルの初期化
void TutorialManager::Init(Game& game)
{
    phase_ = TutorialPhase::WAKE_UP;
    talk_ = 0;
    timer_ = 0;
    move_signal_count_ = 0;

    fade_alpha_ = 0;
    is_fading_out_ = false;
    is_fading_in_ = false;
    fade_wait_start_ = 0;
    is_waiting_ = false;

    dialogue_.Init();
    dialogue_.Show(GameText::Tutorial::WakeUp::kTalks[0]);

    game.SetupTutorialStage();
}

// チュートリアルをスキップする
void TutorialManager::SkipReset()
{
    phase_ = TutorialPhase::TUTORIAL_SKIP;
}

//チュートリアルを終了したか
bool TutorialManager::IsFinished() const
{
    return phase_ == TutorialPhase::TUTORIAL_END;
}

// セリフを表示中かどうか
bool TutorialManager::IsDialogueShowing() const
{
    return dialogue_.IsShowing();
}

// プレイヤー入力を止めるべきフェーズか
bool TutorialManager::IsInputLocked() const
{
    return phase_ == TutorialPhase::WAKE_UP ||
        phase_ == TutorialPhase::NOTICE_RING ||
        phase_ == TutorialPhase::CONSIDER_FIRST_RING ||
        phase_ == TutorialPhase::REACT_TO_FIRST_RING ||
        phase_ == TutorialPhase::REACT_TO_PORTAL ||
        phase_ == TutorialPhase::REACT_TO_WORLD_SHIFT ||
        phase_ == TutorialPhase::WARN_ABOUT_ENEMY ||
        phase_ == TutorialPhase::REACT_TO_ENEMY_HIT ||
        phase_ == TutorialPhase::REFLECT_ON_ENEMY_TRAP ||
        phase_ == TutorialPhase::NOTICE_RING_CHASER ||
        phase_ == TutorialPhase::UNDERSTAND_RING_CHASER ||
        phase_ == TutorialPhase::CONFIRM_RING_CHASER_BEHAVIOR ||
        phase_ == TutorialPhase::FEEL_CORNERED ||
        phase_ == TutorialPhase::UNDERSTAND_SPEED_BOOST ||
        phase_ == TutorialPhase::REACT_TO_MYSTERIOUS_ITEM ||
        phase_ == TutorialPhase::COLLAPSE_WARNING ||
        phase_ == TutorialPhase::FADE_OUT_AND_RESET ||
        phase_ == TutorialPhase::WAKE_AFTER_RESET;
}


// ===== フェーズ更新の判定用 =====

bool TutorialManager::IsPlayerOn(Game& game, int tile_x, int tile_y)
{
    return game.IsPlayerOnTile(tile_x, tile_y);
}

bool TutorialManager::IsChaserOn(Game& game, int tile_x, int tile_y)
{
    return game.IsAnyChaserOnTile(tile_x, tile_y);
}

// ===== チュートリアル 更新 =====

void TutorialManager::Update(Game& game)
{
    switch (phase_) {

    case TutorialPhase::WAKE_UP:
        AdvanceTalkOnEnter(game, talk_);
        ShowTalkLine(dialogue_, talk_, GameText::Tutorial::WakeUp::kTalks);

        if (talk_ == 12) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::MOVE_TO_FIRST_TRIGGER);
        }
        break;

    case TutorialPhase::MOVE_TO_FIRST_TRIGGER:
        if (IsPlayerOn(
            game,
            TutorialConst::kFirstTriggerX,
            TutorialConst::kFirstTriggerY)) {
            phase_ = TutorialPhase::NOTICE_RING;
            talk_ = 1;
        }
        break;

    case TutorialPhase::NOTICE_RING:
        AdvanceTalkOnEnter(game, talk_);
        ShowTalkLine(dialogue_, talk_, GameText::Tutorial::NoticeRing::kTalks);

        if (talk_ == 4) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::MOVE_NEAR_FIRST_RING);
        }
        break;

    case TutorialPhase::MOVE_NEAR_FIRST_RING:
        if (IsPlayerOn(
            game,
            TutorialConst::kFirstRingNearX,
            TutorialConst::kFirstRingNearY)) {
            phase_ = TutorialPhase::CONSIDER_FIRST_RING;
            talk_ = 1;
        }
        break;

    case TutorialPhase::CONSIDER_FIRST_RING:
        AdvanceTalkOnEnter(game, talk_);
        ShowTalkLine(
            dialogue_,
            talk_,
            GameText::Tutorial::ConsiderFirstRing::kTalks);

        if (talk_ == 4) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::STEP_INTO_FIRST_RING);
        }
        break;

    case TutorialPhase::STEP_INTO_FIRST_RING:
        if (IsPlayerOn(
            game,
            TutorialConst::kFirstRingX,
            TutorialConst::kFirstRingY)) {
			game.EnterFirstRingScene();
            phase_ = TutorialPhase::REACT_TO_FIRST_RING;
        }
        break;

    case TutorialPhase::REACT_TO_FIRST_RING:
        AdvanceTalkOnEnter(game, talk_);
        ShowTalkLine(
            dialogue_,
            talk_,
            GameText::Tutorial::ReactToFirstRing::kTalks);

        if (talk_ == 8) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::MOVE_TO_PORTAL);
        }
        break;

    case TutorialPhase::MOVE_TO_PORTAL:
        if (IsPlayerOn(game, TutorialConst::kPortalX, TutorialConst::kPortalY)) {
            phase_ = TutorialPhase::REACT_TO_PORTAL;
        }
        break;

    case TutorialPhase::REACT_TO_PORTAL:
        AdvanceTalkOnEnter(game, talk_);
        ShowTalkLine(dialogue_, talk_, GameText::Tutorial::ReactToPortal::kTalks);

        if (talk_ == 5) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::MOVE_TO_WORLD_SHIFT_RING);
        }
        break;

    case TutorialPhase::MOVE_TO_WORLD_SHIFT_RING:
        if (IsPlayerOn(
            game,
            TutorialConst::kWorldShiftRingX,
            TutorialConst::kWorldShiftRingY)) {
            game.StartTutorialWorldShiftScene();
            phase_ = TutorialPhase::REACT_TO_WORLD_SHIFT;
        }
        break;

    case TutorialPhase::REACT_TO_WORLD_SHIFT:
        AdvanceTalkOnEnter(game, talk_);
        ShowTalkLine(
            dialogue_,
            talk_,
            GameText::Tutorial::ReactToWorldShift::kTalks);

        if (talk_ == 8) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::MOVE_NEAR_SUSPICIOUS_ENEMY);
        }
        break;

    case TutorialPhase::MOVE_NEAR_SUSPICIOUS_ENEMY:
        if (IsPlayerOn(
            game,
            TutorialConst::kSuspiciousEnemyNearX,
            TutorialConst::kSuspiciousEnemyNearY)) {
            phase_ = TutorialPhase::WARN_ABOUT_ENEMY;
        }
        break;

    case TutorialPhase::WARN_ABOUT_ENEMY:
        AdvanceTalkOnEnter(game, talk_);
        ShowTalkLine(dialogue_, talk_, GameText::Tutorial::WarnAboutEnemy::kTalks);

        if (talk_ == 4) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::STEP_TOWARD_ENEMY);
        }
        break;

    case TutorialPhase::STEP_TOWARD_ENEMY:
        if (IsPlayerOn(
            game,
            TutorialConst::kStepTowardEnemyX,
            TutorialConst::kStepTowardEnemyY)) {
            phase_ = TutorialPhase::REACT_TO_ENEMY_HIT;
        }
        break;

    case TutorialPhase::REACT_TO_ENEMY_HIT:
        AdvanceTalkOnEnter(game, talk_);
        ShowSingleTalk(dialogue_, GetReactToEnemyHitTalk(game, talk_));

        if (talk_ == 6) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::MOVE_TO_SAFE_RING);
        }
        break;

    case TutorialPhase::MOVE_TO_SAFE_RING:
        if (IsPlayerOn(
            game,
            TutorialConst::kSafeRingX,
            TutorialConst::kSafeRingY)) {
            phase_ = TutorialPhase::REFLECT_ON_ENEMY_TRAP;
        }
        break;

    case TutorialPhase::REFLECT_ON_ENEMY_TRAP:
        AdvanceTalkOnEnter(game, talk_);
        ShowSingleTalk(dialogue_, GetReflectOnEnemyTrapTalk(game, talk_));

        if (talk_ == 6) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::STEP_INTO_ESCAPE_RING);
        }
        break;

    case TutorialPhase::STEP_INTO_ESCAPE_RING:
        if (IsPlayerOn(
            game,
            TutorialConst::kEscapeRingX,
            TutorialConst::kEscapeRingY)) {
            game.StartTutorialChaserScene();
            phase_ = TutorialPhase::NOTICE_RING_CHASER;
        }
        break;

    case TutorialPhase::NOTICE_RING_CHASER:
        AdvanceTalkOnEnter(game, talk_);
        ShowTalkLine(
            dialogue_,
            talk_,
            GameText::Tutorial::NoticeRingChaser::kTalks);

        if (talk_ == 3) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::WAIT_FOR_RING_CHASER_APPROACH);
        }
        break;

    case TutorialPhase::WAIT_FOR_RING_CHASER_APPROACH:
        if (IsChaserOn(
            game,
            TutorialConst::kChaserApproachX,
            TutorialConst::kChaserApproachY)) {
            phase_ = TutorialPhase::UNDERSTAND_RING_CHASER;
        }
        break;

    case TutorialPhase::UNDERSTAND_RING_CHASER:
        AdvanceTalkOnEnter(game, talk_);
        ShowTalkLine(
            dialogue_,
            talk_,
            GameText::Tutorial::UnderstandRingChaser::kTalks);

        if (talk_ == 3) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::WAIT_FOR_RING_CHASER_REACH_RING);
        }
        break;

    case TutorialPhase::WAIT_FOR_RING_CHASER_REACH_RING:
        if (IsChaserOn(
            game,
            TutorialConst::kChaserReachRingX,
            TutorialConst::kChaserReachRingY)) {
            phase_ = TutorialPhase::CONFIRM_RING_CHASER_BEHAVIOR;
            game.AdvanceTutorialAfterChaserReachedRing();
        }
        break;

    case TutorialPhase::CONFIRM_RING_CHASER_BEHAVIOR:
        AdvanceTalkOnEnter(game, talk_);
        ShowTalkLine(
            dialogue_,
            talk_,
            GameText::Tutorial::ConfirmRingChaserBehavior::kTalks);

        if (talk_ == 2) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::WAIT_FOR_NEXT_RING_MOVE);
        }
        break;

    case TutorialPhase::WAIT_FOR_NEXT_RING_MOVE:
        if (IsChaserOn(game,
            TutorialConst::kAfterChaserReachRingNextX,
            TutorialConst::kAfterChaserReachRingNextY))
        {
            phase_ = TutorialPhase::FEEL_CORNERED;
            game.SetupTutorialCorneredScene();
        }
        break;

    case TutorialPhase::FEEL_CORNERED:
        AdvanceTalkOnEnter(game, talk_);
        ShowTalkLine(dialogue_, talk_, GameText::Tutorial::FeelCornered::kTalks);

        if (talk_ == 5) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::MOVE_TO_MYSTERIOUS_ITEM);
        }
        break;

    case TutorialPhase::MOVE_TO_MYSTERIOUS_ITEM:
        if (IsPlayerOn(
            game,
            TutorialConst::kItemPointAX,
            TutorialConst::kItemPointAY) ||
            IsPlayerOn(
                game,
                TutorialConst::kItemPointBX,
                TutorialConst::kItemPointBY)) {
            phase_ = TutorialPhase::REACT_TO_MYSTERIOUS_ITEM;
        }
        break;

    case TutorialPhase::REACT_TO_MYSTERIOUS_ITEM:
        AdvanceTalkOnEnter(game, talk_);
        ShowTalkLine(
            dialogue_,
            talk_,
            GameText::Tutorial::ReactToMysteriousItem::kTalks);

        if (talk_ == 2) {
            phase_ = TutorialPhase::WAIT_FOR_SPEED_EFFECT;
            talk_ = 1;
            move_signal_count_ = 0;
            dialogue_.Hide();
        }
        break;

    case TutorialPhase::WAIT_FOR_SPEED_EFFECT:
        if (game.GetSignalManager().Has(TutorialSignal::kPlayerMoved)) {
            ++move_signal_count_;
            game.GetSignalManager().Clear(TutorialSignal::kPlayerMoved);
        }

        if (move_signal_count_ > TutorialConst::kSpeedEffectStepThreshold) {
            phase_ = TutorialPhase::UNDERSTAND_SPEED_BOOST;
        }
        break;

    case TutorialPhase::UNDERSTAND_SPEED_BOOST:
        AdvanceTalkOnEnter(game, talk_);
        ShowTalkLine(
            dialogue_,
            talk_,
            GameText::Tutorial::UnderstandSpeedBoost::kTalks);

        if (talk_ == 4) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::REACH_ALPHABET_E);
        }
        break;

    case TutorialPhase::REACH_ALPHABET_E:
        if (game.GetCurrentAlphabet() == 'E') {
            phase_ = TutorialPhase::COLLAPSE_WARNING;
        }
        break;

    case TutorialPhase::COLLAPSE_WARNING:
        AdvanceTalkOnEnter(game, talk_);
        ShowTalkLine(
            dialogue_,
            talk_,
            GameText::Tutorial::CollapseWarning::kTalks);

        if (talk_ == 7) {
            is_fading_out_ = true;
            fade_alpha_ = 0;
            phase_ = TutorialPhase::FADE_OUT_AND_RESET;
            talk_ = 1;
            dialogue_.Hide();
        }
        break;

    case TutorialPhase::FADE_OUT_AND_RESET:
        if (is_fading_out_) {
            fade_alpha_ += TutorialConst::kFadeSpeed;

            if (fade_alpha_ >= TutorialConst::kFadeMaxAlpha) {
                fade_alpha_ = TutorialConst::kFadeMaxAlpha;
                is_fading_out_ = false;

                is_waiting_ = true;
                fade_wait_start_ = GetNowHiPerformanceCount();

                game.ResetAfterTutorialCollapse();
            }
        }
        else if (is_waiting_) {
            const long long now = GetNowHiPerformanceCount();

            if (now - fade_wait_start_ >= TutorialConst::kFadeWaitTime) {
                is_waiting_ = false;
                is_fading_in_ = true;
            }
        }
        else if (is_fading_in_) {
            fade_alpha_ -= TutorialConst::kFadeSpeed;

            if (fade_alpha_ <= 0) {
                fade_alpha_ = 0;
                is_fading_in_ = false;
                phase_ = TutorialPhase::WAKE_AFTER_RESET;
            }
        }
        break;

    case TutorialPhase::WAKE_AFTER_RESET:
        AdvanceTalkOnEnter(game, talk_);
        ShowTalkLine(
            dialogue_,
            talk_,
            GameText::Tutorial::WakeAfterReset::kTalks);

        if (talk_ == 7) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::TUTORIAL_END);
        }
        break;

    case TutorialPhase::TUTORIAL_END:
        break;

    case TutorialPhase::TUTORIAL_SKIP:
        break;
    }
}

// ===== チュートリアル 描画 =====
void TutorialManager::Draw()
{
    dialogue_.Draw();

    if (fade_alpha_ > 0) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, fade_alpha_);
        DrawBox(
            0,
            0,
            GameConst::kScreenWidth,
            GameConst::kScreenHeight,
            GetColor(0, 0, 0),
            TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
}