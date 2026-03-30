#include "Game.h"

#include <cstdio>
#include <cmath>
#include <cstring>

#include "DxLib.h"

#include "GameConstants.h"
#include "DistanceMap.h"
#include "GameTexts.h"
#include "SignalManager.h"

// タイトル画面
void Game::DrawTitle() const
{
	// ===== 背景 =====
    DrawBox(
        0,
        0,
        GameConst::kScreenWidth,
        GameConst::kScreenHeight,
        GetColor(40, 40, 50),
        TRUE);

	// グリッド線
    for (int y = 0; y < GameConst::kScreenHeight; y += GameConst::kTitleGridCellH)
    {
        for (int x = 0; x < GameConst::kScreenWidth; x += GameConst::kTitleGridCellW)
        {
            DrawBox(
                x,
                y,
                x + GameConst::kTitleGridBoxW,
                y + GameConst::kTitleGridBoxH,
                GetColor(60, 60, 70),
                FALSE
            );
        }
    }

	// 浮遊する円
    for (int i = 0; i < GameConst::kTitleCircleCount; ++i) {
        const int x =
            (title_frame_ * GameConst::kTitleFrameSpeedX +
                i * GameConst::kTitleBgCircleSpacingX) % GameConst::kScreenWidth;
        const int y =
            (i * GameConst::kTitleBgCircleSpacingY + title_frame_) % GameConst::kScreenHeight;

        const int gray = GameConst::kTitleBgGrayBase +
            static_cast<int>(
                GameConst::kTitleBgGrayRange *
                (sin((title_frame_ + i * GameConst::kTitleBgWaveOffset) *
                    GameConst::kTitleBgWaveSpeed) + 1.0) * 0.5);

        DrawCircle(
            x,
            y,
            GameConst::kTitleBgCircleRadius,
            GetColor(gray, gray, gray),
            TRUE);
    }

	// ===== ゲームタイトル「ChangeRing O」 =====
    const char* logo = GameText::Title::kLogoMain;
    int draw_x = GameConst::kTitleLogoStartX;
    const int base_y = GameConst::kTitleLogoBaseY;

    for (int i = 0; logo[i] != '\0'; ++i) {
        char c[2] = { logo[i], '\0' };

        const int draw_y = base_y + static_cast<int>(
            sin(title_frame_ * GameConst::kTitleLogoWaveSpeed +
                i * GameConst::kTitleLogoWaveOffset) *
            GameConst::kTitleLogoWaveHeight);

        const int width = GetDrawStringWidthToHandle(c, 1, logo_font_);

        DrawStringToHandle(
            draw_x + GameConst::kTitleLogoShadowOffset,
            draw_y + GameConst::kTitleLogoShadowOffset,
            c,
            GetColor(120, 0, 120),
            logo_font_);

        for (int ox = -GameConst::kTitleLogoOutlineRadius;
            ox <= GameConst::kTitleLogoOutlineRadius;
            ++ox) {

            for (int oy = -GameConst::kTitleLogoOutlineRadius;
                oy <= GameConst::kTitleLogoOutlineRadius;
                ++oy) {

                if (ox * ox + oy * oy <= GameConst::kTitleLogoOutlineDist2) {
                    DrawStringToHandle(
                        draw_x + ox,
                        draw_y + oy,
                        c,
                        GetColor(255, 255, 255),
                        logo_font_);
                }
            }
        }

        const double t = (title_frame_ + i * 10) * 0.05;
        const double wave = (sin(t) + 1.0) * 0.5;

        const int r = static_cast<int>(200 * wave);
        const int g = 200;
        const int b = static_cast<int>(200 * (1.0 - wave));

        DrawStringToHandle(draw_x, draw_y, c, GetColor(r, g, b), logo_font_);
        draw_x += width;
    }

    // ===== サブタイトル「from A to Z」 =====
    {
        const char* sub = GameText::Title::kLogoSub;
        int length = static_cast<int>(strlen(sub));
        int width = GetDrawStringWidthToHandle(sub, length, sub_logo_font_);

        int centerX = GameConst::kScreenCenterX + 20;
        int baseY = base_y + 105;

        for (int i = 0; sub[i] != '\0'; i++)
        {
            char c[2] = { sub[i], '\0' };
            int charWidth = GetDrawStringWidthToHandle(c, 1, sub_logo_font_);

            // 影
            DrawStringToHandle(
                centerX + 4, baseY + 4,
                c,
                GetColor(120, 80, 0),
                sub_logo_font_);

            // 縁取り
            for (int ox = -2; ox <= 2; ox++)
            {
                for (int oy = -2; oy <= 2; oy++)
                {
                    if (ox * ox + oy * oy <= 4)
                    {
                        DrawStringToHandle(
                            centerX + ox, baseY + oy,
                            c,
                            GetColor(255, 255, 255),
                            sub_logo_font_);
                    }
                }
            }

            // 本体（金色アニメ）
            int r = 255;
            int g = static_cast<int>(200 + 30 * sin(title_frame_ * 0.08));
            int b = 50;

            DrawStringToHandle(
                centerX, baseY,
                c,
                GetColor(r, g, b),
                sub_logo_font_);

            centerX += charWidth;
        }
    }

	// ===== 「Press Enter!」 =====
    if ((title_frame_ / GameConst::kTitleBlinkInterval) % 2 == 0) {
        DrawStringToHandle(
            GameConst::kTitlePressX,
            GameConst::kTitlePressY,
            GameText::UI::kPressEnter,
            GetColor(255, 255, 255),
            press_font_);
    }

	// ===== 終了確認ポップアップ =====
    if (isExitPopup_)
    {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);

        DrawBox(
            0,
            0,
            GameConst::kScreenWidth,
            GameConst::kScreenHeight,
            GetColor(0, 0, 0),
            TRUE);

        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

        int boxW = 400;
        int boxH = 200;
        int x = 320 - boxW / 2;
        int y = 240 - boxH / 2;

        DrawBox(x, y, x + boxW, y + boxH, GetColor(0, 0, 0), TRUE);
        DrawBox(x, y, x + boxW, y + boxH, GetColor(255, 255, 255), FALSE);

        DrawString(
            x + 110,
            y + 60,
            GameText::Title::kExitMessage,
            GetColor(255, 255, 255));

        int yesColor = (exitCursor_ == 0) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);
        int noColor = (exitCursor_ == 1) ? GetColor(255, 255, 0) : GetColor(255, 255, 255);

        DrawString(x + 130, y + 130, GameText::UI::kYes, yesColor);
        DrawString(x + 250, y + 130, GameText::UI::kNo, noColor);
    }
}

// チュートリアル画面
void Game::DrawTutorial()
{
    DrawPlay();
    tutorial_manager_.Draw();

    if (!is_skip_popup_) {
        DrawStringToHandle(
            TutorialConst::kSkipHintX,
            TutorialConst::kSkipHintY,
            GameText::UI::kSkip,
            GetColor(255, 255, 255),
            skip_font_);
    }

    if (is_skip_popup_) {
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, TutorialConst::kSkipOverlayAlpha);
        DrawBox(0, 0, GameConst::kScreenWidth, GameConst::kScreenHeight, GetColor(0, 0, 0), TRUE);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

        DrawBox(
            TutorialConst::kSkipPopupX1,
            TutorialConst::kSkipPopupY1,
            TutorialConst::kSkipPopupX2,
            TutorialConst::kSkipPopupY2,
            GetColor(30, 30, 50),
            TRUE);

        DrawBox(
            TutorialConst::kSkipPopupX1,
            TutorialConst::kSkipPopupY1,
            TutorialConst::kSkipPopupX2,
            TutorialConst::kSkipPopupY2,
            GetColor(200, 200, 255),
            FALSE);

        DrawString(
            TutorialConst::kSkipPopupTextX,
            TutorialConst::kSkipPopupTextY,
            GameText::Tutorial::kSkipConfirm,
            GetColor(255, 255, 255));

        DrawString(
            TutorialConst::kSkipYesX,
            TutorialConst::kSkipYesY,
            GameText::UI::kYes,
            skip_index_ == 0 ? GetColor(255, 255, 0) : GetColor(200, 200, 200));

        DrawString(
            TutorialConst::kSkipNoX,
            TutorialConst::kSkipNoY,
            GameText::UI::kNo,
            skip_index_ == 1 ? GetColor(255, 255, 0) : GetColor(200, 200, 200));
    }
}

// ルール説明画面
void Game::DrawGuide() const
{
    DrawBox(0, 0, GameConst::kScreenWidth, GameConst::kScreenHeight, GetColor(15, 15, 25), TRUE);

    const char* title = GameText::Guide::kTitleDecorated;
    const int width = GetDrawStringWidthToHandle(title, -1, guide_title_font_);
    const int x = GameConst::kScreenCenterX - width / 2;

    DrawStringToHandle(
        x + (GameConst::kGuideTitleYShadow - GameConst::kGuideTitleY),
        GameConst::kGuideTitleYShadow,
        title,
        GetColor(0, 0, 0),
        guide_title_font_);
    DrawStringToHandle(
        x,
        GameConst::kGuideTitleY,
        title,
        GetColor(255, 220, 80),
        guide_title_font_);

    DrawString(
        GameConst::kGuideTextX,
        GameConst::kGuideLine1Y,
        GameText::Guide::kLine1,
        GetColor(100, 255, 255));

    DrawString(
        GameConst::kGuideTextX,
        GameConst::kGuideLine2Y,
        GameText::Guide::kLine2,
        GetColor(255, 255, 255));

    DrawString(
        GameConst::kGuideTextX,
        GameConst::kGuideLine3Y,
        GameText::Guide::kLine3,
        GetColor(255, 100, 255));

    DrawString(
        GameConst::kGuideTextX,
        GameConst::kGuideLine4Y,
        GameText::Guide::kLine4,
        GetColor(255, 255, 255));

    DrawString(
        GameConst::kGuideTextX,
        GameConst::kGuideLine5Y,
        GameText::Guide::kLine5,
        GetColor(255, 255, 100));

    if ((guide_frame_ / GameConst::kTitleBlinkInterval) % 2 == 0) {
        DrawStringToHandle(
            GameConst::kGuideStartX,
            GameConst::kGuideStartY,
            GameText::Guide::kStart,
            GetColor(190, 200, 255),
            guide_big_font_);
    }
}

// カウントダウン画面
void Game::DrawCountDown() const
{
    if (count_ > 0) {
        char buf[8];
        sprintf_s(buf, "%d", count_);

        const int width = GetDrawStringWidthToHandle(buf, 1, count_font_);
        const int x = GameConst::kScreenCenterX - width / 2;
        const int y = GameConst::kCountTextY;

        DrawStringToHandle(
            x + GameConst::kCountShadowOffset,
            y + GameConst::kCountShadowOffset,
            buf,
            GetColor(0, 0, 0),
            count_font_);
        DrawStringToHandle(
            x,
            y,
            buf,
            GetColor(255, 255, 255),
            count_font_);
    }
    else {
        const char* text = GameText::Common::kStart;
        const int width = GetDrawStringWidthToHandle(text, 6, count_font_);
        const int x = GameConst::kScreenCenterX - width / 2;
        const int y = GameConst::kCountTextY;

        DrawStringToHandle(
            x + GameConst::kCountShadowOffset,
            y + GameConst::kCountShadowOffset,
            text,
            GetColor(0, 0, 0),
            count_font_);
        DrawStringToHandle(
            x,
            y,
            text,
            GetColor(255, 255, 0),
            count_font_);
    }
}

// プレイ画面
void Game::DrawPlay()
{
    if (is_alt_world_) {
        DrawBox(0, 0, GameConst::kScreenWidth, GameConst::kScreenHeight, GetColor(10, 10, 25), TRUE);
    }
    else {
        DrawBox(0, 0, GameConst::kScreenWidth, GameConst::kScreenHeight, GetColor(255, 40, 50), TRUE);
    }

    map_.Draw();

    for (auto& portal : portals_) {
        portal.Draw();
    }

    for (auto& item : items_) {
        item.Draw();
    }

    ring_.Draw();
    player_.Draw();

    for (const auto& chaser : chasers_) {
        chaser.Draw();
    }

    if (player_.HasDrawPositionChanged()) {
        player_move_count_++;
        signal_manager_.Emit(TutorialSignal::kPlayerMoved);

        if (player_move_count_ >= enemy_move_delay_) {
            player_move_count_ = 0;

            for (auto& enemy : *current_enemies_) {
                enemy.Update(map_);
            }

            DistanceMap::BuildDistanceMapTile(
                ring_.GetTilePosX(),
                ring_.GetTilePosY(),
                map_,
                dist_);

            for (auto& chaser : chasers_) {
                chaser.Update(dist_, map_);
            }

            enemy_move_delay_ = next_enemy_move_delay_;
        }

        for (auto& enemy : *current_enemies_) {
            if (enemy.CheckHit(player_)) {
                DownAlphabet();
                signal_manager_.Emit(TutorialSignal::kPatrolEnemyHit);
                break;
            }
        }

        for (auto& chaser : chasers_) {
            if (chaser.CheckHit(player_)) {
                DownAlphabet();
                signal_manager_.Emit(TutorialSignal::kPatrolEnemyHit);
                break;
            }
        }
    }

    for (auto& enemy : *current_enemies_) {
        enemy.Draw();
    }

    player_.Draw();
    player_.CommitDrawPosition();

    DrawSpawnMessage();
}

// 出現メッセージ
void Game::DrawSpawnMessage() const
{
    if (spawn_message_timer_ <= 0 || spawn_message_[0] == '\0') {
        return;
    }

    const int box_height = 56;
    const int box_y1 = GameConst::kScreenHeight - box_height;
    const int box_y2 = GameConst::kScreenHeight;

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
    DrawBox(
        0,
        box_y1,
        GameConst::kScreenWidth,
        box_y2,
        GetColor(0, 0, 0),
        TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    DrawString(
        20,
        box_y1 + 18,
        spawn_message_,
        GetColor(255, 255, 255));
}

// ゲーム再開画面
void Game::DrawResume() const
{
    for (int y = 0; y < GameConst::kScreenHeight; ++y) {
        DrawLine(0, y, GameConst::kScreenWidth, y, GetColor(10, 10, 10));
    }

    DrawBox(
        GameConst::kResumeBoxX1,
        GameConst::kResumeBoxY1,
        GameConst::kResumeBoxX2,
        GameConst::kResumeBoxY2,
        GetColor(30, 30, 50),
        TRUE);

    DrawBox(
        GameConst::kResumeBoxX1,
        GameConst::kResumeBoxY1,
        GameConst::kResumeBoxX2,
        GameConst::kResumeBoxY2,
        GetColor(80, 80, 150),
        FALSE);

    DrawStringToHandle(
        GameConst::kResumeTitleX,
        GameConst::kResumeTitleY,
        GameText::Resume::kTitle,
        GetColor(250, 220, 0),
        guide_title_font_);

    for (int i = 0; i < 2; ++i) {
        const int y = GameConst::kResumeItemY + i * GameConst::kResumeItemSpacingY;
        const char* text =
            (i == 0) ? GameText::Resume::kContinue : GameText::Resume::kNewGame;

        DrawStringToHandle(
            GameConst::kResumeItemX,
            y - GameConst::kResumeItemTextOffsetY,
            text,
            resume_index_ == i
            ? GetColor(255, 255, 255)
            : GetColor(160, 160, 200),
            guide_font_);
    }
}

// ゲームクリア画面
void Game::DrawClear()
{
    player_.Draw();

    DrawString(
        GameConst::kClearTitleX,
        GameConst::kClearTitleY,
        GameText::Clear::kTitle,
        GetColor(255, 255, 0));

    char buf[64];
    sprintf_s(buf, GameText::Clear::kTimeFormat, clear_time_);

    DrawString(
        GameConst::kClearTimeX,
        GameConst::kClearTimeY,
        buf,
        GetColor(255, 255, 255));

    DrawString(
        GameConst::kClearHintX,
        GameConst::kClearHintY,
        GameText::Clear::kHint,
        GetColor(200, 200, 200));
}