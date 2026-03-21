#include "TutorialManager.h"

#include <string>
#include <vector>

#include "DxLib.h"
#include "Game.h"
#include "Map.h"

namespace {

    const std::vector<PatrolPoint> kRouteTutorialB1 = {
        {14, 7}
    };

    const std::vector<PatrolPoint> kRouteTutorialB2 = {
        {7, 7}, {11, 7}, {5, 7}
    };

    const std::vector<PatrolPoint> kRouteTutorialB3 = {
        {4, 9}, {4, 7}, {6, 7}, {6, 9}
    };

}  // namespace

// ===== DialogueBox =====

void DialogueBox::Init() {
    font_handle_ = CreateFontToHandle(
        nullptr,  // デフォルトフォント
        24,
        3);
}

void DialogueBox::Show(const char* text) {
    current_ = text;
}

void DialogueBox::Hide() {
    current_ = nullptr;
}

bool DialogueBox::IsShowing() const {
    return current_ != nullptr;
}

void DialogueBox::Draw() {
    if (!current_) {
        return;
    }

    // 画面下部 3 タイル分をダイアログ表示領域に使う
    const int box_height = Map::TILE * 3;

    const int box_x1 = 0;
    const int box_y1 = 480 - box_height;
    const int box_x2 = 640;
    const int box_y2 = 480;

    DrawBox(box_x1, box_y1, box_x2, box_y2, GetColor(0, 0, 0), TRUE);
    DrawBox(
        box_x1 + 10,
        box_y1 + 10,
        box_x2 - 10,
        box_y2 - 10,
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
            line.c_str(), static_cast<int>(line.size()), font_handle_);

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

std::vector<std::string> DialogueBox::SplitLines(const char* text) {
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

// ===== TutorialManager =====

void TutorialManager::Init(Game& game) {
    phase_ = TutorialPhase::WAKE_UP;
    talk_ = 0;
    timer_ = 0;
    step = 0;

    fade_alpha_ = 0;
    is_fading_out_ = false;
    is_fading_in_ = false;
    fade_wait_start_ = 0;
    is_waiting_ = false;

    dialogue_.Init();
    dialogue_.Show("………");
    game.SetupTutorialStage();
}

bool TutorialManager::IsPlayerOn(Game& game, int tile_x, int tile_y) {
    return game.player.GetTilePosX() == tile_x &&
        game.player.GetTilePosY() == tile_y;
}

bool TutorialManager::IsEnemyOn(Game& game, int tile_x, int tile_y) {
    for (const auto& chaser : game.chasers) {
        if (chaser.GetTilePosX() == tile_x &&
            chaser.GetTilePosY() == tile_y) {
            return true;
        }
    }
    return false;
}

bool TutorialManager::IsDialogueShowing() const
{
    return dialogue_.IsShowing();
}

void TutorialManager::Update(Game& game) {

    switch (phase_) {

    case TutorialPhase::WAKE_UP:
        if (game.IsKeyTrigger(KEY_INPUT_RETURN)) {
            ++talk_;
        }

        if (talk_ == 0) dialogue_.Show("………");
        if (talk_ == 1) dialogue_.Show("うっ……ここは？");
        if (talk_ == 2) dialogue_.Show("俺は、確か……");
        if (talk_ == 3) dialogue_.Show("…っ！？");
        if (talk_ == 4) dialogue_.Show("かっ、身体が…");
        if (talk_ == 5) dialogue_.Show("アルファベットの『A』になってるっ！？");
        if (talk_ == 6) dialogue_.Show("………");
        if (talk_ == 7) dialogue_.Show("もしかして、俺は夢でも見ているのか？");
        if (talk_ == 8) dialogue_.Show("………");
        if (talk_ == 9) dialogue_.Show("はぁ…悩んでいても、しょうがないか…");
        if (talk_ == 10) dialogue_.Show("なぜだか、動くことはできそうだし");
        if (talk_ == 11) dialogue_.Show("とりあえず、先へ進んでよう！");

        if (talk_ == 12) {
            phase_ = TutorialPhase::MOVE_TO_FIRST_TRIGGER;
            talk_ = 1;
            dialogue_.Hide();
        }
        break;

    case TutorialPhase::MOVE_TO_FIRST_TRIGGER:
        if (IsPlayerOn(game, 9, 5)) {
            phase_ = TutorialPhase::NOTICE_RING;
            talk_ = 1;
        }
        break;

    case TutorialPhase::NOTICE_RING:
        if (game.IsKeyTrigger(KEY_INPUT_RETURN)) {
            ++talk_;
        }

        if (talk_ == 1) dialogue_.Show("んっ……！？");
        if (talk_ == 2) dialogue_.Show("あれは何だ？");
        if (talk_ == 3) dialogue_.Show("光の…リング？");

        if (talk_ == 4) {
            phase_ = TutorialPhase::MOVE_NEAR_FIRST_RING;
            talk_ = 1;
            dialogue_.Hide();
        }
        break;

    case TutorialPhase::MOVE_NEAR_FIRST_RING:
        if (IsPlayerOn(game, 9, 2)) {
            phase_ = TutorialPhase::CONSIDER_FIRST_RING;
            talk_ = 1;
        }
        break;

    case TutorialPhase::CONSIDER_FIRST_RING:
        if (game.IsKeyTrigger(KEY_INPUT_RETURN)) {
            ++talk_;
        }

        if (talk_ == 1) dialogue_.Show("なにか不思議な力を感じるぞ…");
        if (talk_ == 2) dialogue_.Show("ここを通らないと、先には進めそうにないし");
        if (talk_ == 3) dialogue_.Show("この『光のリング』を踏んでみるか…");

        if (talk_ == 4) {
            phase_ = TutorialPhase::STEP_INTO_FIRST_RING;
            talk_ = 1;
            dialogue_.Hide();
        }
        break;

    case TutorialPhase::STEP_INTO_FIRST_RING:
        if (IsPlayerOn(game, 9, 1)) {
            phase_ = TutorialPhase::REACT_TO_FIRST_RING;
        }
        break;

    case TutorialPhase::REACT_TO_FIRST_RING:
        if (game.IsKeyTrigger(KEY_INPUT_RETURN)) {
            ++talk_;
        }

        if (talk_ == 1) dialogue_.Show("身体が『A』から『B』に変わった！？");
        if (talk_ == 2) dialogue_.Show("ってことは…");
        if (talk_ == 3) dialogue_.Show("『Z』で入ったら、どうなるんだ？");
        if (talk_ == 4) dialogue_.Show("あのリングは、向こう側へ移動したみたいだけど…");
        if (talk_ == 5) dialogue_.Show("………");
        if (talk_ == 6) dialogue_.Show("もしかして、あの「青い四角」に入ったら");
        if (talk_ == 7) dialogue_.Show("向こう側へ行けたりして…");

        if (talk_ == 8) {
            phase_ = TutorialPhase::MOVE_TO_PORTAL;
            talk_ = 1;
            dialogue_.Hide();
        }
        break;

    case TutorialPhase::MOVE_TO_PORTAL:
        if (IsPlayerOn(game, 18, 1)) {
            phase_ = TutorialPhase::REACT_TO_PORTAL;
        }
        break;

    case TutorialPhase::REACT_TO_PORTAL:
        if (game.IsKeyTrigger(KEY_INPUT_RETURN)) {
            ++talk_;
        }

        if (talk_ == 1) dialogue_.Show("あっ、本当に移動できた！");
        if (talk_ == 2) dialogue_.Show("この「青い四角」はワープポータルだったのか…");
        if (talk_ == 3) dialogue_.Show("他にできることもないし");
        if (talk_ == 4) dialogue_.Show("ひとまず『Z』を目指して進んでみるか");

        if (talk_ == 5) {
            phase_ = TutorialPhase::MOVE_TO_WORLD_SHIFT_RING;
            talk_ = 1;
            dialogue_.Hide();
        }
        break;

    case TutorialPhase::MOVE_TO_WORLD_SHIFT_RING:
        if (IsPlayerOn(game, 18, 7)) {
            game.portals.clear();
            game.isAltWorld = true;
            game.map.UseTutorialMapB();

            game.ring.SetNextTilePos(1, 7);
            game.ring.RelocateFar(game.player, game.map);

            game.currentEnemies = &game.enemiesB;
            game.enemiesB.emplace_back(kRouteTutorialB1);
            game.enemiesB.emplace_back(kRouteTutorialB2);
            game.enemiesB.emplace_back(kRouteTutorialB3);

            phase_ = TutorialPhase::REACT_TO_WORLD_SHIFT;
        }
        break;

    case TutorialPhase::REACT_TO_WORLD_SHIFT:
        if (game.IsKeyTrigger(KEY_INPUT_RETURN)) {
            ++talk_;
        }

        if (talk_ == 1) dialogue_.Show("はっ……！？");
        if (talk_ == 2) dialogue_.Show("どうなっているんだ…？");
        if (talk_ == 3) dialogue_.Show("今度は「身体」だけじゃなくて");
        if (talk_ == 4) dialogue_.Show("「世界」も変わった！？");
        if (talk_ == 5) dialogue_.Show("本当にどうなっているんだよ…全く");
        if (talk_ == 6) dialogue_.Show("…………");
        if (talk_ == 7) dialogue_.Show("ん………あれは？");

        if (talk_ == 8) {
            phase_ = TutorialPhase::MOVE_NEAR_SUSPICIOUS_ENEMY;
            talk_ = 1;
            dialogue_.Hide();
        }
        break;

    case TutorialPhase::MOVE_NEAR_SUSPICIOUS_ENEMY:
        if (IsPlayerOn(game, 15, 7)) {
            phase_ = TutorialPhase::WARN_ABOUT_ENEMY;
        }
        break;

    case TutorialPhase::WARN_ABOUT_ENEMY:
        if (game.IsKeyTrigger(KEY_INPUT_RETURN)) {
            ++talk_;
        }

        if (talk_ == 1) dialogue_.Show("見るからに怪しい雰囲気を感じるな…");
        if (talk_ == 2) dialogue_.Show("コレは通って大丈夫なのか…？");
        if (talk_ == 3) dialogue_.Show("なんだか嫌な予感がするぞ");

        if (talk_ == 4) {
            phase_ = TutorialPhase::STEP_TOWARD_ENEMY;
            talk_ = 1;
            dialogue_.Hide();
        }
        break;

    case TutorialPhase::STEP_TOWARD_ENEMY:
        if (IsPlayerOn(game, 13, 7)) {
            phase_ = TutorialPhase::REACT_TO_ENEMY_HIT;
        }
        break;

    case TutorialPhase::REACT_TO_ENEMY_HIT:
        if (game.IsKeyTrigger(KEY_INPUT_RETURN)) {
            ++talk_;
        }

        if (game.prevAlphabet == 'A' && game.currentAlphabet == 'A') {
            if (talk_ == 1) dialogue_.Show("戻っちまった…");
            if (talk_ == 2) dialogue_.Show("しかも、何も考えずに何度も踏んだから");
            if (talk_ == 3) dialogue_.Show("最初からやり直しか…");
            if (talk_ == 4) dialogue_.Show("ただ『A』で踏んでも何も起きなかったし");
            if (talk_ == 5) dialogue_.Show("それが分かっただけでも良しとしよう");
        }
        else if (game.prevAlphabet == 'B' && game.currentAlphabet == 'A') {
            if (talk_ == 1) dialogue_.Show("くそ………やっぱり、こうなったか");
            if (talk_ == 2) dialogue_.Show("しかも２回踏んだから『A』に戻ってしまった");
            if (talk_ == 3) dialogue_.Show("もし、もう１度踏んでしまったら…");
            if (talk_ == 4) dialogue_.Show("………");
            if (talk_ == 5) dialogue_.Show("ちょっと怖いし、これからは気を付けないと…");
        }
        else if (game.prevAlphabet == 'C' && game.currentAlphabet == 'B') {
            if (talk_ == 1) dialogue_.Show("………やっぱり");
            if (talk_ == 2) dialogue_.Show("この赤いマークを踏むと");
            if (talk_ == 3) dialogue_.Show("アルファベットが戻ってしまうのか…");
            if (talk_ == 4) dialogue_.Show("しかも、あそこのマークは動くみたいだし");
            if (talk_ == 5) dialogue_.Show("これからは気を付けて進まないとな…");
        }

        if (talk_ == 6) {
            phase_ = TutorialPhase::MOVE_TO_SAFE_RING;
            talk_ = 1;
            dialogue_.Hide();
        }
        break;

    case TutorialPhase::MOVE_TO_SAFE_RING:
        if (IsPlayerOn(game, 2, 7)) {
            phase_ = TutorialPhase::REFLECT_ON_ENEMY_TRAP;
        }
        break;

    case TutorialPhase::REFLECT_ON_ENEMY_TRAP:
        if (game.IsKeyTrigger(KEY_INPUT_RETURN)) {
            ++talk_;
        }

        if (game.prevAlphabet == 'A' && game.currentAlphabet == 'A') {
            if (talk_ == 1) dialogue_.Show("結局、何度も踏んでしまった…");
            if (talk_ == 2) dialogue_.Show("………まあ、いっか……");
            if (talk_ == 3) dialogue_.Show("深く考えてもしょうがないし");
            if (talk_ == 4) dialogue_.Show("『Z』になるまで長くなりそうだしな");
            if (talk_ == 5) dialogue_.Show("とりあえず、進んどけば何とかなるでしょ！");
        }
        else if (game.prevAlphabet == 'B' && game.currentAlphabet == 'A') {
            if (talk_ == 1) dialogue_.Show("結局『A』に戻ってしまった…か");
            if (talk_ == 2) dialogue_.Show("…まぁ、深く考えてもしょうがないし");
            if (talk_ == 3) dialogue_.Show("ここから仕切り直すか…");
            if (talk_ == 4) dialogue_.Show("ただこれ以上何かあっても嫌だし");
            if (talk_ == 5) dialogue_.Show("油断せずに進まないと…");
        }
        else if (game.prevAlphabet == 'C' && game.currentAlphabet == 'B') {
            if (talk_ == 1) dialogue_.Show("ふぅ……");
            if (talk_ == 2) dialogue_.Show("なんとか踏まずにここまで来れた！");
            if (talk_ == 3) dialogue_.Show("とはいえ『Z』までは先が長そうだし");
            if (talk_ == 4) dialogue_.Show("まだまだ何が起こるか分からない");
            if (talk_ == 5) dialogue_.Show("これからも油断せずに進まないと…");
        }

        if (talk_ == 6) {
            phase_ = TutorialPhase::STEP_INTO_ESCAPE_RING;
            talk_ = 1;
            dialogue_.Hide();
        }
        break;

    case TutorialPhase::STEP_INTO_ESCAPE_RING:
        if (IsPlayerOn(game, 1, 7)) {
            game.portals.clear();
            game.currentEnemies->clear();
            game.isAltWorld = false;
            game.map.UseTutorialMapC();

            game.ring.SetNextTilePos(10, 7);
            game.ring.RelocateFar(game.player, game.map);

            game.chasers.emplace_back(18, 7);

            phase_ = TutorialPhase::NOTICE_RING_CHASER;
        }
        break;

    case TutorialPhase::NOTICE_RING_CHASER:
        if (game.IsKeyTrigger(KEY_INPUT_RETURN)) {
            ++talk_;
        }

        if (talk_ == 1) dialogue_.Show("ん…？");
        if (talk_ == 2) dialogue_.Show("今度は、一体どんなヤツだ？");

        if (talk_ == 3) {
            phase_ = TutorialPhase::WAIT_FOR_RING_CHASER_APPROACH;
            talk_ = 1;
            dialogue_.Hide();
        }
        break;

    case TutorialPhase::WAIT_FOR_RING_CHASER_APPROACH:
        if (IsEnemyOn(game, 13, 7)) {
            phase_ = TutorialPhase::UNDERSTAND_RING_CHASER;
        }
        break;

    case TutorialPhase::UNDERSTAND_RING_CHASER:
        if (game.IsKeyTrigger(KEY_INPUT_RETURN)) {
            ++talk_;
        }

        if (talk_ == 1) dialogue_.Show("あいつ、まさか…");
        if (talk_ == 2) dialogue_.Show("リングを追いかけて移動しているのか？");

        if (talk_ == 3) {
            phase_ = TutorialPhase::WAIT_FOR_RING_CHASER_REACH_RING;
            talk_ = 1;
            dialogue_.Hide();
        }
        break;

    case TutorialPhase::WAIT_FOR_RING_CHASER_REACH_RING:
        if (IsEnemyOn(game, 10, 7)) {
            phase_ = TutorialPhase::CONFIRM_RING_CHASER_BEHAVIOR;

            game.ring.SetNextTilePos(15, 5);
            game.ring.RelocateFar(game.player, game.map);
        }
        break;

    case TutorialPhase::CONFIRM_RING_CHASER_BEHAVIOR:
        if (game.IsKeyTrigger(KEY_INPUT_RETURN)) {
            ++talk_;
        }

        if (talk_ == 1) dialogue_.Show("やっぱり、そうだったのか…");

        if (talk_ == 2) {
            phase_ = TutorialPhase::WAIT_FOR_NEXT_RING_MOVE;
            talk_ = 1;
            dialogue_.Hide();
        }
        break;

    case TutorialPhase::WAIT_FOR_NEXT_RING_MOVE:
        if (IsEnemyOn(game, 15, 5)) {
            phase_ = TutorialPhase::FEEL_CORNERED;

            game.ring.SetNextTilePos(11, 11);
            game.ring.RelocateFar(game.player, game.map);
            game.ring.next_x_ = 0;
            game.ring.next_y_ = 0;

            game.items.emplace_back(11, 4, ItemType::SLOW_ENEMY);
            game.items.emplace_back(8, 9, ItemType::SLOW_ENEMY);
        }
        break;

    case TutorialPhase::FEEL_CORNERED:
        if (game.IsKeyTrigger(KEY_INPUT_RETURN)) {
            ++talk_;
        }

        if (talk_ == 1) dialogue_.Show("くそ、このままじゃ…");
        if (talk_ == 2) dialogue_.Show("いつまで経っても先に進めない…");
        if (talk_ == 3) dialogue_.Show("一体…どうすれば…");
        if (talk_ == 4) dialogue_.Show("ん…？　あれは、何だ？");

        if (talk_ == 5) {
            phase_ = TutorialPhase::MOVE_TO_MYSTERIOUS_ITEM;
            talk_ = 1;
            dialogue_.Hide();
        }
        break;

    case TutorialPhase::MOVE_TO_MYSTERIOUS_ITEM:
        if (IsPlayerOn(game, 11, 4) || IsPlayerOn(game, 8, 9)) {
            phase_ = TutorialPhase::REACT_TO_MYSTERIOUS_ITEM;
        }
        break;

    case TutorialPhase::REACT_TO_MYSTERIOUS_ITEM:
        if (game.IsKeyTrigger(KEY_INPUT_RETURN)) {
            ++talk_;
        }

        if (talk_ == 1) dialogue_.Show("触れても、特に何も起きなかったけど…");

        if (talk_ == 2) {
            phase_ = TutorialPhase::WAIT_FOR_SPEED_EFFECT;
            talk_ = 1;
            step = 0;
            dialogue_.Hide();
        }
        break;

    case TutorialPhase::WAIT_FOR_SPEED_EFFECT:
        if (step > 2) {
            phase_ = TutorialPhase::UNDERSTAND_SPEED_BOOST;
        }
        break;

    case TutorialPhase::UNDERSTAND_SPEED_BOOST:
        if (game.IsKeyTrigger(KEY_INPUT_RETURN)) {
            ++talk_;
        }

        if (talk_ == 1) dialogue_.Show("なるほど…");
        if (talk_ == 2) dialogue_.Show("アイツよりも少しだけ速く動けるようになったみたいだ");
        if (talk_ == 3) dialogue_.Show("これなら何とかなるかもしれないぞ");

        if (talk_ == 4) {
            phase_ = TutorialPhase::REACH_ALPHABET_E;
            talk_ = 1;
            dialogue_.Hide();
        }
        break;

    case TutorialPhase::REACH_ALPHABET_E:
        if (game.currentAlphabet == 'E') {
            phase_ = TutorialPhase::COLLAPSE_WARNING;
        }
        break;

    case TutorialPhase::COLLAPSE_WARNING:
        if (game.IsKeyTrigger(KEY_INPUT_RETURN)) {
            ++talk_;
        }

        if (talk_ == 1) dialogue_.Show("よし、この調子でどんどん進んでいこう！");
        if (talk_ == 2) dialogue_.Show("（グラッ…）");
        if (talk_ == 3) dialogue_.Show("うっ…急に意識が…");
        if (talk_ == 4) dialogue_.Show("くそっ、これからってときに…");
        if (talk_ == 5) dialogue_.Show("時間をかけすぎちまったって…ことなの…かっ…");
        if (talk_ == 6) dialogue_.Show("……………");

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
            fade_alpha_ += 8;

            if (fade_alpha_ >= 255) {
                fade_alpha_ = 255;
                is_fading_out_ = false;

                is_waiting_ = true;
                fade_wait_start_ = GetNowHiPerformanceCount();

                game.ResetGame();
                game.currentAlphabet = 'A';
                game.prevAlphabet = '\0';

                game.map.UseMapA();
                game.isAltWorld = false;

                game.player.SetTilePos(10, 7);

                game.enemiesA.clear();
                game.enemiesB.clear();
                game.chasers.clear();
                game.portals.clear();
                game.items.clear();

                game.ring.RelocateFar(game.player, game.map);
            }
        }
        else if (is_waiting_) {
            const long long now = GetNowHiPerformanceCount();

            if (now - fade_wait_start_ >= 2000000) {
                is_waiting_ = false;
                is_fading_in_ = true;
            }
        }
        else if (is_fading_in_) {
            fade_alpha_ -= 8;

            if (fade_alpha_ <= 0) {
                fade_alpha_ = 0;
                is_fading_in_ = false;
                phase_ = TutorialPhase::WAKE_AFTER_RESET;
            }
        }
        break;

    case TutorialPhase::WAKE_AFTER_RESET:
        if (game.IsKeyTrigger(KEY_INPUT_RETURN)) {
            ++talk_;
        }

        if (talk_ == 1) dialogue_.Show("……………");
        if (talk_ == 2) dialogue_.Show("うっ……ここは…");
        if (talk_ == 3) dialogue_.Show("……………");
        if (talk_ == 4) dialogue_.Show("やっぱり…『A』に戻ってしまったか");
        if (talk_ == 5) dialogue_.Show("………まあ、仕方ねぇな");
        if (talk_ == 6) dialogue_.Show("気を取り直して、また『Z』を目指そう！");

        if (talk_ == 7) {
            phase_ = TutorialPhase::TUTORIAL_END;
            talk_ = 1;
            dialogue_.Hide();
        }
        break;

    case TutorialPhase::TUTORIAL_END:
        break;
    }
}

void TutorialManager::Draw() {
    dialogue_.Draw();

    if (fade_alpha_ > 0) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, fade_alpha_);
        DrawBox(0, 0, 640, 480, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    }
}

bool TutorialManager::IsFinished() const {
    return phase_ == TutorialPhase::TUTORIAL_END;
}

void TutorialManager::SkipReset()
{
    phase_ = TutorialPhase::TUTORIAL_SKIP;
}

bool TutorialManager::IsInputLocked() const {
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
        phase_ == TutorialPhase::REACT_TO_MYSTERIOUS_ITEM ||
        phase_ == TutorialPhase::UNDERSTAND_SPEED_BOOST ||
        phase_ == TutorialPhase::COLLAPSE_WARNING;
}