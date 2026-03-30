#include "PatrolEnemy.h"

#include <vector>

#include "DxLib.h"
#include "Map.h"
#include "Player.h"
#include "GameConstants.h"

namespace
{
    // 4方向
    const int kDx[4] = { 1, -1, 0, 0 };
    const int kDy[4] = { 0, 0, 1, -1 };

    // 自動生成ルートの長さ
    constexpr int kAutoRouteMinLength = 8;
    constexpr int kAutoRouteMaxLength = 14;
}

// 巡回ルートを指定して敵を生成する
PatrolEnemy::PatrolEnemy(const std::vector<PatrolPoint>& route)
    : patrol_route_(route)
{
    if (!patrol_route_.empty()) {
        SetTilePos(patrol_route_[0].x, patrol_route_[0].y);
    }
}

// 巡回ルートを後から設定する
void PatrolEnemy::SetPatrolRoute(const std::vector<PatrolPoint>& route)
{
    patrol_route_ = route;
    current_index_ = 0;

    if (!patrol_route_.empty()) {
        SetTilePos(patrol_route_[0].x, patrol_route_[0].y);
    }
}

// 巡回ルートを持っているか
bool PatrolEnemy::HasPatrolRoute() const
{
    return !patrol_route_.empty();
}

// 現在の巡回ルートを返す
const std::vector<PatrolPoint>& PatrolEnemy::GetPatrolRoute() const
{
    return patrol_route_;
}

// ルート未指定なら、その場で自動生成する
void PatrolEnemy::EnsureAutoRoute(const Map& map)
{
    if (HasPatrolRoute()) {
        return;
    }

    GenerateAutoRoute(map);
}

void PatrolEnemy::SetTilePos(int tile_x, int tile_y)
{
    GridObject::SetTileCenterPos(tile_x, tile_y);
}

bool PatrolEnemy::IsInside(int tile_x, int tile_y) const
{
    return tile_x >= 1 && tile_y >= 1 &&
        tile_x < Map::W - 1 && tile_y < Map::H - 1;
}

bool PatrolEnemy::IsWalkableTile(const Map& map, int tile_x, int tile_y) const
{
    if (!IsInside(tile_x, tile_y)) {
        return false;
    }

    return map.IsWalkableTile(tile_x, tile_y);
}

bool PatrolEnemy::ContainsPoint(
    const std::vector<PatrolPoint>& route,
    int tile_x,
    int tile_y) const
{
    for (const auto& point : route) {
        if (point.x == tile_x && point.y == tile_y) {
            return true;
        }
    }

    return false;
}

// ルート未指定時にマップから巡回ルートを生成する
void PatrolEnemy::GenerateAutoRoute(const Map& map)
{
    std::vector<PatrolPoint> start_candidates;

    // 開始候補を集める
    for (int y = 1; y < Map::H - 1; ++y) {
        for (int x = 1; x < Map::W - 1; ++x) {
            if (!IsWalkableTile(map, x, y)) {
                continue;
            }

            int neighbor_count = 0;
            for (int i = 0; i < 4; ++i) {
                const int nx = x + kDx[i];
                const int ny = y + kDy[i];

                if (IsWalkableTile(map, nx, ny)) {
                    ++neighbor_count;
                }
            }

            // 一本道すぎない場所を開始候補にする
            if (neighbor_count >= 2) {
                start_candidates.push_back({ x, y });
            }
        }
    }

    if (start_candidates.empty()) {
        return;
    }

    const PatrolPoint start =
        start_candidates[GetRand(static_cast<int>(start_candidates.size()) - 1)];

    // ===== 片道ルートを作る =====
    std::vector<PatrolPoint> one_way_route;
    one_way_route.push_back(start);

    int current_x = start.x;
    int current_y = start.y;

    int prev_x = -1;
    int prev_y = -1;

    const int target_length =
        kAutoRouteMinLength +
        GetRand(kAutoRouteMaxLength - kAutoRouteMinLength + 1);

    for (int step = 0; step < target_length; ++step) {
        std::vector<PatrolPoint> next_candidates;

        for (int i = 0; i < 4; ++i) {
            const int nx = current_x + kDx[i];
            const int ny = current_y + kDy[i];

            if (!IsWalkableTile(map, nx, ny)) {
                continue;
            }

            // 直前のマスへの即戻りを避ける
            if (nx == prev_x && ny == prev_y) {
                continue;
            }

            // 同じマスの使い回しを避ける
            if (ContainsPoint(one_way_route, nx, ny)) {
                continue;
            }

            next_candidates.push_back({ nx, ny });
        }

        // 候補がなければ片道ルート作成終了
        if (next_candidates.empty()) {
            break;
        }

        const PatrolPoint next =
            next_candidates[GetRand(static_cast<int>(next_candidates.size()) - 1)];

        prev_x = current_x;
        prev_y = current_y;
        current_x = next.x;
        current_y = next.y;

        one_way_route.push_back(next);
    }

    // 短すぎる場合はその場停止
    if (one_way_route.size() < 2) {
        patrol_route_.clear();
        patrol_route_.push_back(start);
        current_index_ = 0;
        SetTilePos(start.x, start.y);
        return;
    }

    // ===== 往復ルートにして「最後→先頭」も隣接にする =====
    patrol_route_.clear();

    // 行き
    for (const auto& point : one_way_route) {
        patrol_route_.push_back(point);
    }

    // 帰り（終点と始点は重複させない）
    for (int i = static_cast<int>(one_way_route.size()) - 2; i >= 1; --i) {
        patrol_route_.push_back(one_way_route[i]);
    }

    current_index_ = 0;
    SetTilePos(patrol_route_[0].x, patrol_route_[0].y);
}

// 巡回ルートに沿って敵を移動させる
void PatrolEnemy::Update(const Map& map)
{
    // 巡回ルートがなければ移動しない
    if (patrol_route_.empty()) {
        return;
    }

    // 現在向かっている目標地点
    const PatrolPoint& target = patrol_route_[current_index_];

    const int current_tile_x = GetTilePosX();
    const int current_tile_y = GetTilePosY();

    int move_x = 0;
    int move_y = 0;

    // 目標地点に向かって1マス分だけ移動方向を決める
    // x方向を優先し、xが一致したらy方向へ進む
    if (target.x > current_tile_x) {
        move_x = 1;
    }
    else if (target.x < current_tile_x) {
        move_x = -1;
    }
    else if (target.y > current_tile_y) {
        move_y = 1;
    }
    else if (target.y < current_tile_y) {
        move_y = -1;
    }

    const int next_x = x_ + move_x * MapConst::kMapTile;
    const int next_y = y_ + move_y * MapConst::kMapTile;

    const int next_left = next_x - PatrolEnemyConst::kWidth / 2;
    const int next_top = next_y - PatrolEnemyConst::kHeight / 2;

    // 次の移動先が通行可能なら移動する
    if (map.IsWalkableRect(next_left, next_top, PatrolEnemyConst::kWidth , PatrolEnemyConst::kHeight)) {
        x_ = next_x;
        y_ = next_y;

        dir_x_ = move_x;
        dir_y_ = move_y;
    }

    // 目標地点に到達していて移動しない場合は、下向きを維持する
    if (move_x == 0 && move_y == 0) {
        dir_x_ = PatrolEnemyConst::kIdleDirX;
        dir_y_ = PatrolEnemyConst::kIdleDirY;
    }

    // 現在の目標地点に到達したら、次の巡回先へ進む
    if (GetTilePosX() == target.x && GetTilePosY() == target.y) {
        current_index_ = (current_index_ + 1) % patrol_route_.size();
    }
}

// プレイヤーと同じマスにいるか
bool PatrolEnemy::CheckHit(const Player& player) const
{
    return IsOnTile(player.GetTilePosX(), player.GetTilePosY());
}

// 敵を進行方向に向けて描画する
void PatrolEnemy::Draw() const
{
    // 敵本体
    DrawCircle(
        x_,
        y_,
        PatrolEnemyConst::kBodyRadius,
        GetColor(
            PatrolEnemyConst::kBodyColorR,
            PatrolEnemyConst::kBodyColorG,
            PatrolEnemyConst::kBodyColorB),
        TRUE);

    const int half_width = PatrolEnemyConst::kWidth / 2;
    const int half_height = PatrolEnemyConst::kHeight / 2;

    // ===== 前方の三角形 =====

    int offset_x = -dir_x_ * half_width / PatrolEnemyConst::kOffsetDiv;
    int offset_y = -dir_y_ * half_height / PatrolEnemyConst::kOffsetDiv;

    int center_x = x_ - dir_x_ * half_width / PatrolEnemyConst::kCenterDiv - offset_x;
    int center_y = y_ - dir_y_ * half_height / PatrolEnemyConst::kCenterDiv - offset_y;

    int tip_x = center_x + dir_x_ * half_width;
    int tip_y = center_y + dir_y_ * half_height;

    int left_x = center_x - dir_y_ * half_width / PatrolEnemyConst::kTriangleSideDiv;
    int left_y = center_y + dir_x_ * half_height / PatrolEnemyConst::kTriangleSideDiv;

    int right_x = center_x + dir_y_ * half_width / PatrolEnemyConst::kTriangleSideDiv;
    int right_y = center_y - dir_x_ * half_height / PatrolEnemyConst::kTriangleSideDiv;

    DrawTriangle(
        tip_x, tip_y,
        left_x, left_y,
        right_x, right_y,
        GetColor(
            PatrolEnemyConst::kTriangleColorR,
            PatrolEnemyConst::kTriangleColorG,
            PatrolEnemyConst::kTriangleColorB),
        FALSE);

    // ===== 後方の三角形 =====

    offset_x = dir_x_ * half_width / PatrolEnemyConst::kOffsetDiv;
    offset_y = dir_y_ * half_height / PatrolEnemyConst::kOffsetDiv;

    center_x = x_ + dir_x_ * half_width / PatrolEnemyConst::kCenterDiv - offset_x;
    center_y = y_ + dir_y_ * half_height / PatrolEnemyConst::kCenterDiv - offset_y;

    tip_x = center_x - dir_x_ * half_width;
    tip_y = center_y - dir_y_ * half_height;

    left_x = center_x + dir_y_ * half_width / PatrolEnemyConst::kTriangleSideDiv;
    left_y = center_y - dir_x_ * half_height / PatrolEnemyConst::kTriangleSideDiv;

    right_x = center_x - dir_y_ * half_width / PatrolEnemyConst::kTriangleSideDiv;
    right_y = center_y + dir_x_ * half_height / PatrolEnemyConst::kTriangleSideDiv;

    DrawTriangle(
        tip_x, tip_y,
        left_x, left_y,
        right_x, right_y,
        GetColor(
            PatrolEnemyConst::kTriangleColorR,
            PatrolEnemyConst::kTriangleColorG,
            PatrolEnemyConst::kTriangleColorB),
        FALSE);
}