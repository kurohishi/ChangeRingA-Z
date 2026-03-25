#include "PatrolEnemy.h"

#include "DxLib.h"
#include "Map.h"
#include "Player.h"
#include "Constants.h"

// ===== PatrolEnemy =====

// 巡回ルートを指定して敵を生成する
PatrolEnemy::PatrolEnemy(const std::vector<PatrolPoint>& route)
    : patrol_route_(route)
{
    if (!patrol_route_.empty()) {
        SetTilePos(patrol_route_[0].x, patrol_route_[0].y);
    }
}

void PatrolEnemy::SetTilePos(int tile_x, int tile_y)
{
    GridObject::SetTileCenterPos(tile_x, tile_y);
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

    const int next_x = x_ + move_x * Map::TILE;
    const int next_y = y_ + move_y * Map::TILE;

    const int next_left = next_x - width_ / 2;
    const int next_top = next_y - height_ / 2;

    // 次の移動先が通行可能なら移動する
    if (map.IsWalkableRect(next_left, next_top, width_, height_)) {
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

// プレイヤーと同じタイルにいるかを判定する
bool PatrolEnemy::CheckHit(const Player& player) const
{
    return IsOnTile(player.GetTilePosX(), player.GetTilePosY());
}

// 敵を進行方向に合わせて描画する
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

    const int half_width = width_ / 2;
    const int half_height = height_ / 2;

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
    // 前後対称の見た目にするため、反対側にも描画する

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