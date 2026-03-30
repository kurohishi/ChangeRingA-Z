#pragma once

// ==============================
//              共通基盤
// ==============================

namespace CommonConst
{
    // ===== 無効値 =====
    constexpr int kInvalidIndex = -1;
    constexpr int kInvalidHandle = -1;

    // ===== 画面サイズ =====
    constexpr int kScreenWidth = 640;
    constexpr int kScreenHeight = 480;
    constexpr int kScreenCenterX = kScreenWidth / 2;
    constexpr int kScreenCenterY = kScreenHeight / 2;

    // ===== 時間 =====
    constexpr long long kOneSecondMicroseconds = 1000000LL;
    constexpr long long kTwoSecondsMicroseconds = 2000000LL;

    // ===== キャラクター共通サイズ =====
    constexpr int kCharacterWidth = 24;
    constexpr int kCharacterHeight = 24;

    // ===== 初期方向 =====
    constexpr int kDirRight = 1;
    constexpr int kDirLeft = -1;
    constexpr int kDirUp = -1;
    constexpr int kDirDown = 1;
    constexpr int kDirNone = 0;

    // ===== 色 =====
    constexpr int kColorBlackR = 0;
    constexpr int kColorBlackG = 0;
    constexpr int kColorBlackB = 0;

    constexpr int kColorWhiteR = 255;
    constexpr int kColorWhiteG = 255;
    constexpr int kColorWhiteB = 255;
}

// ==============================
//              ゲーム関連
// ==============================

namespace GameConst
{
    // ===== ゲーム =====
    constexpr int kInvalidIndex = CommonConst::kInvalidIndex;
    constexpr int kInvalidFontHandle = CommonConst::kInvalidHandle;

    // ===== 入力 =====
    constexpr int kKeyBufferSize = 256;

    // ===== ワープ =====
    constexpr long long kWaitMicroseconds = CommonConst::kOneSecondMicroseconds;

    // ===== アルファベット =====
    constexpr char kInitialAlphabet = 'A';
    constexpr char kNoAlphabet = '\0';

    // ===== 画面サイズ =====
    constexpr int kScreenWidth = CommonConst::kScreenWidth;
    constexpr int kScreenHeight = CommonConst::kScreenHeight;
    constexpr int kScreenCenterX = CommonConst::kScreenCenterX;
    constexpr int kScreenCenterY = CommonConst::kScreenCenterY;

    // ===== ゲーム進行 =====
    constexpr int kInitialCountdown = 3;
    constexpr long long kCountdownInterval = CommonConst::kOneSecondMicroseconds;

    // ===== 敵移動 =====
    constexpr int kNormalEnemyMoveDelay = 2;
    constexpr int kGameStartEnemyMoveDelay = 1;
    constexpr int kGameStartNextEnemyMoveDelay = 1;
    constexpr int kPortalCooldownFrames = 5;

    // ===== アイテム効果 =====
    constexpr int kItemEffectDurationFrames = 120;

    // ===== アルファベット変化 =====
    constexpr int kAlphabetNormalStep = 1;
    constexpr int kAlphabetBoostStep = 2;
    constexpr char kAlphabetStart = 'A';
    constexpr char kAlphabetGoal = 'Z';

    // ===== 敵出現数 =====
    constexpr int kFirstEnemyCount = 1;
    constexpr int kSecondEnemyCount = 2;
    constexpr int kThirdChaserCount = 3;

    // ===== 敵出現条件 =====
    constexpr char kSpawnPatrol1At = 'D';
    constexpr char kSpawnPatrol2At = 'G';
    constexpr char kSpawnChaser1At = 'J';
    constexpr char kSpawnBoostItemAt = 'M';
    constexpr char kSpawnChaser2At = 'P';
    constexpr char kSpawnPortalAt = 'S';
    constexpr char kSpawnChaser3At = 'V';

    // ===== BFS / 距離マップ =====
    constexpr int kBfsDirectionCount = 4;
    constexpr int kDistUnreachable = -1;

    // ===== タイトル画面 =====

    // 背景グリッド
    constexpr int kTitleGridCellW = 80;
    constexpr int kTitleGridCellH = 40;
    constexpr int kTitleGridBoxW = 78;
    constexpr int kTitleGridBoxH = 38;

    // 背景の円
    constexpr int kTitleCircleCount = 20;
    constexpr int kTitleBgCircleSpacingX = 60;
    constexpr int kTitleBgCircleSpacingY = 40;
    constexpr int kTitleBgCircleRadius = 25;

    constexpr int kTitleFrameSpeedX = 2;
    constexpr int kTitleBgWaveOffset = 10;
    constexpr double kTitleBgWaveSpeed = 0.05;
    constexpr int kTitleBgGrayBase = 40;
    constexpr int kTitleBgGrayRange = 60;

    // ロゴ
    constexpr int kTitleLogoStartX = 100;
    constexpr int kTitleLogoBaseY = 170;
    constexpr int kTitleLogoShadowOffset = 6;
    constexpr int kTitleLogoOutlineRadius = 3;
    constexpr int kTitleLogoOutlineDist2 = 9;
    constexpr double kTitleLogoWaveSpeed = 0.08;
    constexpr double kTitleLogoWaveOffset = 0.4;
    constexpr int kTitleLogoWaveHeight = 8;

    // Press Enter
    constexpr int kTitlePressX = 250;
    constexpr int kTitlePressY = 350;
    constexpr int kTitleBlinkInterval = 30;

    // ===== ガイド画面 =====
    constexpr int kGuideTitleYShadow = 100;
    constexpr int kGuideTitleY = 80;

    constexpr int kGuideLine1Y = 170;
    constexpr int kGuideLine2Y = 210;
    constexpr int kGuideLine3Y = 250;
    constexpr int kGuideLine4Y = 290;
    constexpr int kGuideLine5Y = 330;

    constexpr int kGuideTextX = 60;
    constexpr int kGuideStartX = 150;
    constexpr int kGuideStartY = 390;

    // ===== カウントダウン画面 =====
    constexpr int kCountTextY = 180;
    constexpr int kCountShadowOffset = 6;

    // ===== プレイ画面 =====
    constexpr int kSpawnMessageDurationFrames = 180;

    // ===== ゲーム再開画面 =====
    constexpr int kResumeBgColorStep = 1;

    constexpr int kResumeBoxX1 = 120;
    constexpr int kResumeBoxY1 = 170;
    constexpr int kResumeBoxX2 = 520;
    constexpr int kResumeBoxY2 = 350;

    constexpr int kResumeTitleX = 100;
    constexpr int kResumeTitleY = 80;

    constexpr int kResumeItemX = 250;
    constexpr int kResumeItemY = 220;
    constexpr int kResumeItemSpacingY = 60;
    constexpr int kResumeItemTextOffsetY = 5;

    // ===== クリア画面 =====
    constexpr int kClearTitleX = 220;
    constexpr int kClearTitleY = 160;

    constexpr int kClearTimeX = 180;
    constexpr int kClearTimeY = 210;

    constexpr int kClearHintX = 140;
    constexpr int kClearHintY = 260;
}

// ==============================
//              フォント関連
// ==============================

namespace FontConst
{
    constexpr int kInvalidHandle = CommonConst::kInvalidHandle;

    constexpr int kLogoSize = 72;
    constexpr int kLogoThickness = 6;

    constexpr int kPressSize = 28;
    constexpr int kPressThickness = 3;

    constexpr int kSubLogoSize = 35;
    constexpr int kSubLogoThickness = 6;

    constexpr int kTutorialBigSize = 36;
    constexpr int kTutorialBigThickness = 6;

    constexpr int kSkipSize = 17;
    constexpr int kSkipThickness = 3;

    constexpr int kGuideTitleSize = 48;
    constexpr int kGuideTitleThickness = 6;

    constexpr int kGuideSize = 28;
    constexpr int kGuideThickness = 3;

    constexpr int kGuideBigSize = 30;
    constexpr int kGuideBigThickness = 6;

    constexpr int kCountSize = 80;
    constexpr int kCountThickness = 6;

    constexpr int kDialogueSize = 24;
    constexpr int kDialogueThickness = 3;
}

// ==============================
//              UI共通
// ==============================

namespace UIConst
{
    constexpr int kMarginX = 16;
    constexpr int kMarginY = 16;
}

// ==============================
//              チュートリアル関連
// ==============================

namespace TutorialConst
{
    // ===== UI / ダイアログ =====
    constexpr int kDialogueBoxTileHeight = 3;
    constexpr int kDialogueBorderMargin = 10;

    // ===== スキップポップアップ =====
    constexpr int kSkipHintX = 530;
    constexpr int kSkipHintY = 10;

    constexpr int kSkipOverlayAlpha = 150;

    constexpr int kSkipPopupX1 = 150;
    constexpr int kSkipPopupY1 = 150;
    constexpr int kSkipPopupX2 = 490;
    constexpr int kSkipPopupY2 = 300;

    constexpr int kSkipPopupTextX = 180;
    constexpr int kSkipPopupTextY = 180;

    constexpr int kSkipYesX = 240;
    constexpr int kSkipYesY = 230;

    constexpr int kSkipNoX = 360;
    constexpr int kSkipNoY = 230;

    // ===== 演出 =====
    constexpr int kFadeSpeed = 8;
    constexpr int kFadeMaxAlpha = 255;
    constexpr long long kFadeWaitTime = CommonConst::kTwoSecondsMicroseconds;

    // ===== 初期配置 =====
    constexpr int kPlayerStartX = 9;
    constexpr int kPlayerStartY = 7;

    constexpr int kRingStartX = 9;
    constexpr int kRingStartY = 1;

    constexpr int kRingNextX = 18;
    constexpr int kRingNextY = 7;

    constexpr int kChaserStartX = 18;
    constexpr int kChaserStartY = 7;

    constexpr int kPortalAX = 1;
    constexpr int kPortalAY = 1;
    constexpr int kPortalAPair = 1;

    constexpr int kPortalBX = 18;
    constexpr int kPortalBY = 1;
    constexpr int kPortalBPair = 0;

    // ===== リング移動先 =====
    constexpr int kWorldShiftNextRingX = 1;
    constexpr int kWorldShiftNextRingY = 7;

    constexpr int kAfterChaserNextRingX = 15;
    constexpr int kAfterChaserNextRingY = 5;

    constexpr int kCorneredNextRingX = 11;
    constexpr int kCorneredNextRingY = 11;

    constexpr int kEndNextRingX = 10;
    constexpr int kEndNextRingY = 2;

    // ===== 進行座標 =====
    constexpr int kFirstTriggerX = 9;
    constexpr int kFirstTriggerY = 5;

    constexpr int kFirstRingNearX = 9;
    constexpr int kFirstRingNearY = 2;

    constexpr int kFirstRingX = 9;
    constexpr int kFirstRingY = 1;

    constexpr int kPortalX = 18;
    constexpr int kPortalY = 1;

    constexpr int kWorldShiftRingX = 18;
    constexpr int kWorldShiftRingY = 7;

    constexpr int kSuspiciousEnemyNearX = 15;
    constexpr int kSuspiciousEnemyNearY = 7;

    constexpr int kStepTowardEnemyX = 13;
    constexpr int kStepTowardEnemyY = 7;

    constexpr int kSafeRingX = 2;
    constexpr int kSafeRingY = 7;

    constexpr int kEscapeRingX = 1;
    constexpr int kEscapeRingY = 7;

    constexpr int kChaserApproachX = 13;
    constexpr int kChaserApproachY = 7;

    constexpr int kChaserReachRingX = 10;
    constexpr int kChaserReachRingY = 7;

    constexpr int kItemPointAX = 11;
    constexpr int kItemPointAY = 4;

    constexpr int kItemPointBX = 8;
    constexpr int kItemPointBY = 9;

    constexpr int kResetPlayerX = 10;
    constexpr int kResetPlayerY = 7;

    constexpr int kAfterChaserReachRingNextX = 15;
    constexpr int kAfterChaserReachRingNextY = 5;

    constexpr int kSpeedEffectStepThreshold = 2;
}

// ==============================
//              プレイヤー関連
// ==============================

namespace PlayerConst
{
    constexpr int kWidth = CommonConst::kCharacterWidth;
    constexpr int kHeight = CommonConst::kCharacterHeight;
    constexpr char kNoCharacter = '\0';

    constexpr int kStartX = 335;
    constexpr int kStartY = 240;

    constexpr int kSpeed = 1;
    constexpr int kFontSize = 24;

    constexpr int kDrawOffsetXDiv = 10;
    constexpr int kDrawOffsetYDiv = 4;

    constexpr int kMoveIntervalFrames = 30;
}

// ==============================
//              マップ関連
// ==============================

namespace MapConst
{
    constexpr int kMapTile = 32;

    constexpr int kWall = 0;
    constexpr int kFloor = 1;

    constexpr int kMapWidth = 20;
    constexpr int kMapHeight = 15;

    constexpr int kWallColorR = 80;
    constexpr int kWallColorG = 80;
    constexpr int kWallColorB = 80;

    constexpr int kAltWallColorR = 120;
    constexpr int kAltWallColorG = 40;
    constexpr int kAltWallColorB = 120;

    constexpr int kFloorColorR = 20;
    constexpr int kFloorColorG = 20;
    constexpr int kFloorColorB = 20;

    constexpr int kAltFloorColorR = 10;
    constexpr int kAltFloorColorG = 10;
    constexpr int kAltFloorColorB = 30;

    constexpr int kGridColorR = 100;
    constexpr int kGridColorG = 100;
    constexpr int kGridColorB = 100;

    constexpr int kFallbackTileX = 10;
    constexpr int kFallbackTileY = 7;
}

// ==============================
//              アイテム関連
// ==============================

namespace ItemConst
{
    constexpr double kColorWaveSpeed = 0.05;
    constexpr double kColorWaveBase = 1.0;
    constexpr double kColorWaveScale = 0.5;

    constexpr int kOuterRadius = 11;
    constexpr int kOuterInnerRadius = 10;
    constexpr int kMiddleRadius = 8;
    constexpr int kInnerRadius = 6;
    constexpr int kCoreRadius = 4;

    constexpr int kOuterHalfLength = 11;
    constexpr int kOuterInnerHalfLength = 10;
    constexpr int kMiddleHalfLength = 8;
    constexpr int kInnerHalfLength = 7;
    constexpr int kCoreHalfLength = 6;

    constexpr int kFrameR = 200;
    constexpr int kFrameG = 180;
    constexpr int kFrameB = 255;

    constexpr int kBlackR = CommonConst::kColorBlackR;
    constexpr int kBlackG = CommonConst::kColorBlackG;
    constexpr int kBlackB = CommonConst::kColorBlackB;

    constexpr int kWhiteR = CommonConst::kColorWhiteR;
    constexpr int kWhiteG = CommonConst::kColorWhiteG;
    constexpr int kWhiteB = CommonConst::kColorWhiteB;

    constexpr int kBaseColor = 200;

    constexpr int kBoostFixedR = 255;
    constexpr int kBoostFixedG = 255;
    constexpr int kBoostFixedB = 0;

    constexpr float kAnimSpeed = 0.05f;
}

// ==============================
//              リング関連
// ==============================

namespace RingConst
{
    constexpr int kParticleCount = 12;
    constexpr float kParticleSpeed = 0.01f;

    constexpr int kRadius = 16;
    constexpr int kStartX = 200;
    constexpr int kStartY = 200;

    constexpr int kNoNextPosition = 0;
    constexpr int kMinRelocateDistance = 150;

    constexpr int kInnerRingRadiusDiv = 2;
    constexpr int kCoreRadius = 4;
    constexpr int kParticleRadius = 1;
    constexpr int kPulseOffset = 3;
    constexpr float kPulseSpeed = 2.0f;

    constexpr int kRingColorR = 255;
    constexpr int kRingColorG = 255;
    constexpr int kRingColorB = 0;

    constexpr int kCoreColorR = 0;
    constexpr int kCoreColorG = 255;
    constexpr int kCoreColorB = 0;

    constexpr int kParticleColorR = 100;
    constexpr int kParticleColorG = 255;
    constexpr int kParticleColorB = 255;
}

// ==============================
//              巡回敵関連
// ==============================

namespace PatrolEnemyConst
{
    constexpr int kWidth = CommonConst::kCharacterWidth;
    constexpr int kHeight = CommonConst::kCharacterHeight;

    constexpr int kInitialDirX = CommonConst::kDirRight;
    constexpr int kInitialDirY = CommonConst::kDirDown;

    constexpr int kIdleDirX = CommonConst::kDirNone;
    constexpr int kIdleDirY = CommonConst::kDirDown;

    constexpr int kSpeed = 10;

    constexpr int kBodyRadius = 12;

    constexpr int kOffsetDiv = 5;
    constexpr int kCenterDiv = 2;
    constexpr int kTriangleSideDiv = 2;

    constexpr int kBodyColorR = 50;
    constexpr int kBodyColorG = 0;
    constexpr int kBodyColorB = 50;

    constexpr int kTriangleColorR = 255;
    constexpr int kTriangleColorG = 0;
    constexpr int kTriangleColorB = 0;
}

// ==============================
//              リング追跡敵関連
// ==============================

namespace RingChaserConst
{
    constexpr int kWidth = CommonConst::kCharacterWidth;
    constexpr int kHeight = CommonConst::kCharacterHeight;

    constexpr int kInitialDirX = CommonConst::kDirLeft;
    constexpr int kInitialDirY = CommonConst::kDirNone;

    constexpr int kStartX = 0;
    constexpr int kStartY = 0;

    constexpr int kIdleDirX = CommonConst::kDirNone;
    constexpr int kIdleDirY = CommonConst::kDirDown;

    constexpr int kDirectionCount = 4;
    constexpr int kDeltaX[kDirectionCount] = { 1, -1, 0, 0 };
    constexpr int kDeltaY[kDirectionCount] = { 0, 0, 1, -1 };

    constexpr int kOffsetDiv = 5;
    constexpr int kCenterDiv = 2;
    constexpr int kTriangleSideDiv = 3;

    constexpr int kTriangleColorR = 255;
    constexpr int kTriangleColorG = 255;
    constexpr int kTriangleColorB = 0;
}

// ==============================
//              ワープポータル関連
// ==============================

namespace PortalConst
{
    constexpr float kAnimSpeed = 0.01f;
    constexpr float kAnimLoopMax = 1.0f;
    constexpr float kSecondWaveOffset = 0.5f;

    constexpr int kColorR = 0;
    constexpr int kColorG = 255;
    constexpr int kColorB = 255;
}

// ==============================
//              サウンド関連
// ==============================

namespace SoundConst
{
    constexpr int kNoBgm = CommonConst::kInvalidIndex;
    constexpr int kDefaultVolume = 100;
}