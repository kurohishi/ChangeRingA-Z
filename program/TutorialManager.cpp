#include "TutorialManager.h"

#include <string>
#include <vector>
#include <initializer_list>

#include "DxLib.h"
#include "Game.h"
#include "Constants.h"
#include "Map.h"

namespace {

    // ===== 巡回ルート定義 =====

    const std::vector<PatrolPoint> kRouteTutorialB1 = {
        {14, 7}
    };

    const std::vector<PatrolPoint> kRouteTutorialB2 = {
        {7, 7}, {11, 7}, {5, 7}
    };

    const std::vector<PatrolPoint> kRouteTutorialB3 = {
        {4, 9}, {4, 7}, {6, 7}, {6, 9}
    };

    // ===== チュートリアル専用座標 =====

    constexpr int kWorldShiftNextRingX = 1;
    constexpr int kWorldShiftNextRingY = 7;

    constexpr int kChaserStartX = 18;
    constexpr int kChaserStartY = 7;

    constexpr int kAfterChaserReachRingNextX = 15;
    constexpr int kAfterChaserReachRingNextY = 5;

    constexpr int kCorneredNextRingX = 11;
    constexpr int kCorneredNextRingY = 11;

    constexpr int kSpeedEffectStepThreshold = 2;

    // ===== 会話進行用補助 =====

    void AdvanceTalkOnEnter(Game& game, int& talk)
    {
        if (game.IsKeyTrigger(KEY_INPUT_RETURN)) {
            ++talk;
        }
    }

    void ShowTalkLine(
        DialogueBox& dialogue,
        int talk,
        std::initializer_list<const char*> lines)
    {
        const std::vector<const char*> line_list(lines);

        if (talk >= 0 && talk < static_cast<int>(line_list.size())) {
            dialogue.Show(line_list[talk]);
        }
    }

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

}  // namespace

// ===== DialogueBox =====

void DialogueBox::Init()
{
    font_handle_ = CreateFontToHandle(
        nullptr,
        FontConst::kDialogueSize,
        FontConst::kDialogueThickness);
}

void DialogueBox::Show(const char* text)
{
    current_ = text;
}

void DialogueBox::Hide()
{
    current_ = nullptr;
}

bool DialogueBox::IsShowing() const
{
    return current_ != nullptr;
}

void DialogueBox::Draw()
{
    if (!current_) {
        return;
    }

    const int box_height = Map::TILE * TutorialConst::kDialogueBoxTileHeight;

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

// ===== TutorialManager 基本 =====

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
    dialogue_.Show("………");

    game.SetupTutorialStage();
}

void TutorialManager::SkipReset()
{
    phase_ = TutorialPhase::TUTORIAL_SKIP;
}

bool TutorialManager::IsFinished() const
{
    return phase_ == TutorialPhase::TUTORIAL_END;
}

bool TutorialManager::IsDialogueShowing() const
{
    return dialogue_.IsShowing();
}

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

void TutorialManager::Draw()
{
    dialogue_.Draw();

    if (fade_alpha_ > 0) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, fade_alpha_);
        DrawBox(0, 0, GameConst::kScreenWidth, GameConst::kScreenHeight, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
}

// ===== TutorialManager 補助 =====

bool TutorialManager::IsPlayerOn(Game& game, int tile_x, int tile_y)
{
    return game.IsPlayerOnTile(tile_x, tile_y);
}

bool TutorialManager::IsChaserOn(Game& game, int tile_x, int tile_y)
{
    return game.IsAnyChaserOnTile(tile_x, tile_y);
}

// ===== TutorialManager 更新 =====

void TutorialManager::Update(Game& game)
{
    switch (phase_) {

    case TutorialPhase::WAKE_UP:
        AdvanceTalkOnEnter(game, talk_);

        ShowTalkLine(dialogue_, talk_, {
            "………",
            "うっ……ここは？",
            "俺は、確か……",
            "…っ！？",
            "かっ、身体が…",
            "アルファベットの『A』になってるっ！？",
            "………",
            "もしかして、俺は夢でも見ているのか？",
            "………",
            "はぁ…悩んでいても、しょうがないか…",
            "なぜだか、動くことはできそうだし",
            "とりあえず、先へ進んでよう！"
            });

        if (talk_ == 12) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::MOVE_TO_FIRST_TRIGGER);
        }
        break;

    case TutorialPhase::MOVE_TO_FIRST_TRIGGER:
        if (IsPlayerOn(game, TutorialConst::kFirstTriggerX, TutorialConst::kFirstTriggerY)) {
            phase_ = TutorialPhase::NOTICE_RING;
            talk_ = 1;
        }
        break;

    case TutorialPhase::NOTICE_RING:
        AdvanceTalkOnEnter(game, talk_);

        switch (talk_) {
        case 1: dialogue_.Show("んっ……！？"); break;
        case 2: dialogue_.Show("あれは何だ？"); break;
        case 3: dialogue_.Show("光の…リング？"); break;
        }

        if (talk_ == 4) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::MOVE_NEAR_FIRST_RING);
        }
        break;

    case TutorialPhase::MOVE_NEAR_FIRST_RING:
        if (IsPlayerOn(game, TutorialConst::kFirstRingNearX, TutorialConst::kFirstRingNearY)) {
            phase_ = TutorialPhase::CONSIDER_FIRST_RING;
            talk_ = 1;
        }
        break;

    case TutorialPhase::CONSIDER_FIRST_RING:
        AdvanceTalkOnEnter(game, talk_);

        switch (talk_) {
        case 1: dialogue_.Show("なにか不思議な力を感じるぞ…"); break;
        case 2: dialogue_.Show("ここを通らないと、先には進めそうにないし"); break;
        case 3: dialogue_.Show("この『光のリング』を踏んでみるか…"); break;
        }

        if (talk_ == 4) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::STEP_INTO_FIRST_RING);
        }
        break;

    case TutorialPhase::STEP_INTO_FIRST_RING:
        if (IsPlayerOn(game, TutorialConst::kFirstRingX, TutorialConst::kFirstRingY)) {
            phase_ = TutorialPhase::REACT_TO_FIRST_RING;
        }
        break;

    case TutorialPhase::REACT_TO_FIRST_RING:
        AdvanceTalkOnEnter(game, talk_);

        switch (talk_) {
        case 1: dialogue_.Show("身体が『A』から『B』に変わった！？"); break;
        case 2: dialogue_.Show("ってことは…"); break;
        case 3: dialogue_.Show("『Z』で入ったら、どうなるんだ？"); break;
        case 4: dialogue_.Show("あのリングは、向こう側へ移動したみたいだけど…"); break;
        case 5: dialogue_.Show("………"); break;
        case 6: dialogue_.Show("もしかして、あの「青い四角」に入ったら"); break;
        case 7: dialogue_.Show("向こう側へ行けたりして…"); break;
        }

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

        switch (talk_) {
        case 1: dialogue_.Show("あっ、本当に移動できた！"); break;
        case 2: dialogue_.Show("この「青い四角」はワープポータルだったのか…"); break;
        case 3: dialogue_.Show("他にできることもないし"); break;
        case 4: dialogue_.Show("ひとまず『Z』を目指して進んでみるか"); break;
        }

        if (talk_ == 5) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::MOVE_TO_WORLD_SHIFT_RING);
        }
        break;

    case TutorialPhase::MOVE_TO_WORLD_SHIFT_RING:
        if (IsPlayerOn(game, TutorialConst::kWorldShiftRingX, TutorialConst::kWorldShiftRingY)) {
            game.StartTutorialWorldShiftScene();
            phase_ = TutorialPhase::REACT_TO_WORLD_SHIFT;
        }
        break;

    case TutorialPhase::REACT_TO_WORLD_SHIFT:
        AdvanceTalkOnEnter(game, talk_);

        switch (talk_) {
        case 1: dialogue_.Show("はっ……！？"); break;
        case 2: dialogue_.Show("どうなっているんだ…？"); break;
        case 3: dialogue_.Show("今度は「身体」だけじゃなくて"); break;
        case 4: dialogue_.Show("「世界」も変わった！？"); break;
        case 5: dialogue_.Show("本当にどうなっているんだよ…全く"); break;
        case 6: dialogue_.Show("…………"); break;
        case 7: dialogue_.Show("ん………あれは？"); break;
        }

        if (talk_ == 8) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::MOVE_NEAR_SUSPICIOUS_ENEMY);
        }
        break;

    case TutorialPhase::MOVE_NEAR_SUSPICIOUS_ENEMY:
        if (IsPlayerOn(game, TutorialConst::kSuspiciousEnemyNearX, TutorialConst::kSuspiciousEnemyNearY)) {
            phase_ = TutorialPhase::WARN_ABOUT_ENEMY;
        }
        break;

    case TutorialPhase::WARN_ABOUT_ENEMY:
        AdvanceTalkOnEnter(game, talk_);

        switch (talk_) {
        case 1: dialogue_.Show("見るからに怪しい雰囲気を感じるな…"); break;
        case 2: dialogue_.Show("コレは通って大丈夫なのか…？"); break;
        case 3: dialogue_.Show("なんだか嫌な予感がするぞ"); break;
        }

        if (talk_ == 4) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::STEP_TOWARD_ENEMY);
        }
        break;

    case TutorialPhase::STEP_TOWARD_ENEMY:
        if (IsPlayerOn(game, TutorialConst::kStepTowardEnemyX, TutorialConst::kStepTowardEnemyY)) {
            phase_ = TutorialPhase::REACT_TO_ENEMY_HIT;
        }
        break;

    case TutorialPhase::REACT_TO_ENEMY_HIT:
        AdvanceTalkOnEnter(game, talk_);

        if (game.GetPreviousAlphabet() == 'A' && game.GetCurrentAlphabet() == 'A') {
            switch (talk_) {
            case 1: dialogue_.Show("戻っちまった…"); break;
            case 2: dialogue_.Show("しかも、何も考えずに何度も踏んだから"); break;
            case 3: dialogue_.Show("最初からやり直しか…"); break;
            case 4: dialogue_.Show("ただ『A』で踏んでも何も起きなかったし"); break;
            case 5: dialogue_.Show("それが分かっただけでも良しとしよう"); break;
            }
        }
        else if (game.GetPreviousAlphabet() == 'B' && game.GetCurrentAlphabet() == 'A') {
            switch (talk_) {
            case 1: dialogue_.Show("くそ………やっぱり、こうなったか"); break;
            case 2: dialogue_.Show("しかも２回踏んだから『A』に戻ってしまった"); break;
            case 3: dialogue_.Show("もし、もう１度踏んでしまったら…"); break;
            case 4: dialogue_.Show("………"); break;
            case 5: dialogue_.Show("ちょっと怖いし、これからは気を付けないと…"); break;
            }
        }
        else if (game.GetPreviousAlphabet() == 'C' && game.GetCurrentAlphabet() == 'B') {
            switch (talk_) {
            case 1: dialogue_.Show("………やっぱり"); break;
            case 2: dialogue_.Show("この赤いマークを踏むと"); break;
            case 3: dialogue_.Show("アルファベットが戻ってしまうのか…"); break;
            case 4: dialogue_.Show("しかも、あそこのマークは動くみたいだし"); break;
            case 5: dialogue_.Show("これからは気を付けて進まないとな…"); break;
            }
        }

        if (talk_ == 6) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::MOVE_TO_SAFE_RING);
        }
        break;

    case TutorialPhase::MOVE_TO_SAFE_RING:
        if (IsPlayerOn(game, TutorialConst::kSafeRingX, TutorialConst::kSafeRingY)) {
            phase_ = TutorialPhase::REFLECT_ON_ENEMY_TRAP;
        }
        break;

    case TutorialPhase::REFLECT_ON_ENEMY_TRAP:
        AdvanceTalkOnEnter(game, talk_);

        if (game.GetPreviousAlphabet() == 'A' && game.GetCurrentAlphabet() == 'A') {
            switch (talk_) {
            case 1: dialogue_.Show("結局、何度も踏んでしまった…"); break;
            case 2: dialogue_.Show("………まあ、いっか……"); break;
            case 3: dialogue_.Show("深く考えてもしょうがないし"); break;
            case 4: dialogue_.Show("『Z』になるまで長くなりそうだしな"); break;
            case 5: dialogue_.Show("とりあえず、進んどけば何とかなるでしょ！"); break;
            }
        }
        else if (game.GetPreviousAlphabet() == 'B' && game.GetCurrentAlphabet() == 'A') {
            switch (talk_) {
            case 1: dialogue_.Show("結局『A』に戻ってしまった…か"); break;
            case 2: dialogue_.Show("…まぁ、深く考えてもしょうがないし"); break;
            case 3: dialogue_.Show("ここから仕切り直すか…"); break;
            case 4: dialogue_.Show("ただこれ以上何かあっても嫌だし"); break;
            case 5: dialogue_.Show("油断せずに進まないと…"); break;
            }
        }
        else if (game.GetPreviousAlphabet() == 'C' && game.GetCurrentAlphabet() == 'B') {
            switch (talk_) {
            case 1: dialogue_.Show("ふぅ……"); break;
            case 2: dialogue_.Show("なんとか踏まずにここまで来れた！"); break;
            case 3: dialogue_.Show("とはいえ『Z』までは先が長そうだし"); break;
            case 4: dialogue_.Show("まだまだ何が起こるか分からない"); break;
            case 5: dialogue_.Show("これからも油断せずに進まないと…"); break;
            }
        }

        if (talk_ == 6) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::STEP_INTO_ESCAPE_RING);
        }
        break;

    case TutorialPhase::STEP_INTO_ESCAPE_RING:
        if (IsPlayerOn(game, TutorialConst::kEscapeRingX, TutorialConst::kEscapeRingY)) {
            game.StartTutorialChaserScene();
            phase_ = TutorialPhase::NOTICE_RING_CHASER;
        }
        break;

    case TutorialPhase::NOTICE_RING_CHASER:
        AdvanceTalkOnEnter(game, talk_);

        switch (talk_) {
        case 1: dialogue_.Show("ん…？"); break;
        case 2: dialogue_.Show("今度は、一体どんなヤツだ？"); break;
        }

        if (talk_ == 3) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::WAIT_FOR_RING_CHASER_APPROACH);
        }
        break;

    case TutorialPhase::WAIT_FOR_RING_CHASER_APPROACH:
        if (IsChaserOn(game, TutorialConst::kChaserApproachX, TutorialConst::kChaserApproachY)) {
            phase_ = TutorialPhase::UNDERSTAND_RING_CHASER;
        }
        break;

    case TutorialPhase::UNDERSTAND_RING_CHASER:
        AdvanceTalkOnEnter(game, talk_);

        switch (talk_) {
        case 1: dialogue_.Show("あいつ、まさか…"); break;
        case 2: dialogue_.Show("リングを追いかけて移動しているのか？"); break;
        }

        if (talk_ == 3) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::WAIT_FOR_RING_CHASER_REACH_RING);
        }
        break;

    case TutorialPhase::WAIT_FOR_RING_CHASER_REACH_RING:
        if (IsChaserOn(game, TutorialConst::kChaserReachRingX, TutorialConst::kChaserReachRingY)) {
            phase_ = TutorialPhase::CONFIRM_RING_CHASER_BEHAVIOR;
            game.AdvanceTutorialAfterChaserReachedRing();
        }
        break;

    case TutorialPhase::CONFIRM_RING_CHASER_BEHAVIOR:
        AdvanceTalkOnEnter(game, talk_);

        if (talk_ == 1) {
            dialogue_.Show("やっぱり、そうだったのか…");
        }

        if (talk_ == 2) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::WAIT_FOR_NEXT_RING_MOVE);
        }
        break;

    case TutorialPhase::WAIT_FOR_NEXT_RING_MOVE:
        if (IsChaserOn(game, 15, 5)) {
            phase_ = TutorialPhase::FEEL_CORNERED;
            game.SetupTutorialCorneredScene();
        }
        break;

    case TutorialPhase::FEEL_CORNERED:
        AdvanceTalkOnEnter(game, talk_);

        switch (talk_) {
        case 1: dialogue_.Show("くそ、このままじゃ…"); break;
        case 2: dialogue_.Show("いつまで経っても先に進めない…"); break;
        case 3: dialogue_.Show("一体…どうすれば…"); break;
        case 4: dialogue_.Show("ん…？　あれは、何だ？"); break;
        }

        if (talk_ == 5) {
            ChangePhase(
                phase_,
                talk_,
                dialogue_,
                TutorialPhase::MOVE_TO_MYSTERIOUS_ITEM);
        }
        break;

    case TutorialPhase::MOVE_TO_MYSTERIOUS_ITEM:
        if (IsPlayerOn(game, TutorialConst::kItemPointAX, TutorialConst::kItemPointAY) ||
            IsPlayerOn(game, TutorialConst::kItemPointBX, TutorialConst::kItemPointBY)) {
            phase_ = TutorialPhase::REACT_TO_MYSTERIOUS_ITEM;
        }
        break;

    case TutorialPhase::REACT_TO_MYSTERIOUS_ITEM:
        AdvanceTalkOnEnter(game, talk_);

        if (talk_ == 1) {
            dialogue_.Show("触れても、特に何も起きなかったけど…");
        }

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

        if (move_signal_count_ > kSpeedEffectStepThreshold) {
            phase_ = TutorialPhase::UNDERSTAND_SPEED_BOOST;
        }
        break;

    case TutorialPhase::UNDERSTAND_SPEED_BOOST:
        AdvanceTalkOnEnter(game, talk_);

        switch (talk_) {
        case 1: dialogue_.Show("なるほど…"); break;
        case 2: dialogue_.Show("アイツよりも少しだけ速く動けるようになったみたいだ"); break;
        case 3: dialogue_.Show("これなら何とかなるかもしれないぞ"); break;
        }

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

        switch (talk_) {
        case 1: dialogue_.Show("よし、この調子でどんどん進んでいこう！"); break;
        case 2: dialogue_.Show("（グラッ…）"); break;
        case 3: dialogue_.Show("うっ…急に意識が…"); break;
        case 4: dialogue_.Show("くそっ、これからってときに…"); break;
        case 5: dialogue_.Show("時間をかけすぎちまったって…ことなの…かっ…"); break;
        case 6: dialogue_.Show("……………"); break;
        }

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

        switch (talk_) {
        case 1: dialogue_.Show("……………"); break;
        case 2: dialogue_.Show("うっ……ここは…"); break;
        case 3: dialogue_.Show("……………"); break;
        case 4: dialogue_.Show("やっぱり…『A』に戻ってしまったか"); break;
        case 5: dialogue_.Show("………まあ、仕方ねぇな"); break;
        case 6: dialogue_.Show("気を取り直して、また『Z』を目指そう！"); break;
        }

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