#include "Game.h"

#include "GameConstants.h"

// チュートリアルの敵巡回ルート
namespace
{
    const std::vector<PatrolPoint> kTutorialRouteB1 = {
        {14, 7}
    };

    const std::vector<PatrolPoint> kTutorialRouteB2 = {
        {7, 7}, {11, 7}, {5, 7}
    };

    const std::vector<PatrolPoint> kTutorialRouteB3 = {
        {4, 9}, {4, 7}, {6, 7}, {6, 9}
    };
}

// チュートリアルステージの初期化
void Game::SetupTutorialStage()
{
    map_.UseTutorialMapA();
    map_.SetAltWorld(false);
    is_alt_world_ = false;

    player_.SetTilePos(
        TutorialConst::kPlayerStartX,
        TutorialConst::kPlayerStartY);

    ring_.SetTilePos(
        TutorialConst::kRingStartX,
        TutorialConst::kRingStartY);

    ring_.SetNextTilePos(
        TutorialConst::kRingNextX,
        TutorialConst::kRingNextY);

    portals_.clear();
    portals_.emplace_back(
        TutorialConst::kPortalAX,
        TutorialConst::kPortalAY,
        TutorialConst::kPortalAPair);
    portals_.emplace_back(
        TutorialConst::kPortalBX,
        TutorialConst::kPortalBY,
        TutorialConst::kPortalBPair);

    chasers_.clear();
    items_.clear();

    signal_manager_.ClearAll();
}

// 指定マスにプレイヤーがいるか
bool Game::IsPlayerOnTile(int tile_x, int tile_y) const
{
    return player_.GetTilePosX() == tile_x &&
        player_.GetTilePosY() == tile_y;
}

// 指定マスに敵がいるか
bool Game::IsAnyChaserOnTile(int tile_x, int tile_y) const
{
    for (const auto& chaser : chasers_) {
        if (chaser.GetTilePosX() == tile_x &&
            chaser.GetTilePosY() == tile_y) {
            return true;
        }
    }
    return false;
}

// 現在の表示文字を返す
char Game::GetCurrentAlphabet() const
{
    return current_alphabet_;
}

// 前回の表示文字を返す
char Game::GetPreviousAlphabet() const
{
    return prev_alphabet_;
}

// 最初のリングに接触するシーン
void Game::EnterFirstRingScene()
{
    ring_.SetNextTilePos(
        TutorialConst::kRingNextX,
        TutorialConst::kRingNextY);
    ring_.RelocateFar(player_, map_);
}

// 初めて裏世界を体験するシーン
void Game::StartTutorialWorldShiftScene()
{
    portals_.clear();
    is_alt_world_ = true;
    map_.UseTutorialMapB();

    ring_.SetNextTilePos(
        TutorialConst::kWorldShiftNextRingX,
        TutorialConst::kWorldShiftNextRingY);
    ring_.RelocateFar(player_, map_);

    current_enemies_ = &enemies_b_;
    enemies_b_.clear();
    enemies_b_.emplace_back(kTutorialRouteB1);
    enemies_b_.emplace_back(kTutorialRouteB2);
    enemies_b_.emplace_back(kTutorialRouteB3);
}

// 追跡者が登場するシーン
void Game::StartTutorialChaserScene()
{
    portals_.clear();

    if (current_enemies_ != nullptr) {
        current_enemies_->clear();
    }

    is_alt_world_ = false;
    map_.UseTutorialMapC();

    ring_.SetNextTilePos(
        TutorialConst::kChaserReachRingX,
        TutorialConst::kChaserReachRingY);
    ring_.RelocateFar(player_, map_);

    chasers_.clear();
    chasers_.emplace_back(
        TutorialConst::kChaserStartX,
        TutorialConst::kChaserStartY);
}

// 追跡者がリングに到達した後のシーン
void Game::AdvanceTutorialAfterChaserReachedRing()
{
    ring_.SetNextTilePos(
        TutorialConst::kAfterChaserNextRingX,
        TutorialConst::kAfterChaserNextRingY);
    ring_.RelocateFar(player_, map_);
}

// 追い詰められるシーン
void Game::SetupTutorialCorneredScene()
{
    ring_.SetNextTilePos(
        TutorialConst::kCorneredNextRingX,
        TutorialConst::kCorneredNextRingY);
    ring_.RelocateFar(player_, map_);
    ring_.ClearNextPos();

    items_.clear();
    items_.emplace_back(
        TutorialConst::kItemPointAX,
        TutorialConst::kItemPointAY,
        ItemType::SLOW_ENEMY);
    items_.emplace_back(
        TutorialConst::kItemPointBX,
        TutorialConst::kItemPointBY,
        ItemType::SLOW_ENEMY);
}

// プレイヤーの状態がリセットされるシーン
void Game::ResetAfterTutorialCollapse()
{
    ResetGame();
    current_alphabet_ = GameConst::kAlphabetStart;
    prev_alphabet_ = '\0';

    map_.UseAllRoadMap();
    is_alt_world_ = false;

    player_.SetTilePos(
        TutorialConst::kResetPlayerX,
        TutorialConst::kResetPlayerY);

    enemies_a_.clear();
    enemies_b_.clear();
    chasers_.clear();
    portals_.clear();
    items_.clear();

    ring_.SetTilePos(
        TutorialConst::kEndNextRingX,
        TutorialConst::kEndNextRingY);
    ring_.SetNextTilePos(
        TutorialConst::kRingNextX,
        TutorialConst::kRingNextY);
}