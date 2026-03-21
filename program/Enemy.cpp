#include "Enemy.h"

#include "DxLib.h"
#include "Map.h"

Enemy::Enemy(const std::vector<PatrolPoint>& route) {

    patrol_route_ = route;
    current_index_ = 0;

    // 巡回ルートがある場合は、最初の地点を初期位置にする
    // PatrolPoint はタイル座標なので、内部保持用にピクセル中心座標へ変換する
    if (!route.empty()) {
        x_ = route[0].x * Map::TILE + Map::TILE / 2;
        y_ = route[0].y * Map::TILE + Map::TILE / 2;
    }

    // 初期状態では下向きを正面としておく
    dir_x_ = 0;
    dir_y_ = 1;

    width_ = Map::TILE;
    height_ = Map::TILE;
    speed_ = 1000;
}

void Enemy::Update(const Map& map) {

    // 巡回ルートがなければ移動しない
    if (patrol_route_.empty()) {
        return;
    }

    // 現在向かっている巡回先
    const PatrolPoint target = patrol_route_[current_index_];

    const int tile_x = GetTilePosX();
    const int tile_y = GetTilePosY();

    int delta_x = 0;
    int delta_y = 0;

    // 目標地点に向かって、1回の更新で1マス分だけ移動方向を決める
    // x方向を優先し、xが一致しているときだけy方向へ進む
    if (target.x > tile_x) {
        delta_x = 1;
    }
    else if (target.x < tile_x) {
        delta_x = -1;
    }
    else if (target.y > tile_y) {
        delta_y = 1;
    }
    else if (target.y < tile_y) {
        delta_y = -1;
    }

    const int next_pixel_x = x_ + delta_x * Map::TILE;
    const int next_pixel_y = y_ + delta_y * Map::TILE;

    const int next_left = next_pixel_x - width_ / 2;
    const int next_top = next_pixel_y - height_ / 2;

    // 次の移動先が通行可能なら、実際に移動する
    if (map.IsWalkableRect(next_left, next_top, width_, height_)) {
        x_ = next_pixel_x;
        y_ = next_pixel_y;

        dir_x_ = delta_x;
        dir_y_ = delta_y;
    }

    // すでに目標地点に到達していて移動しない場合は、下向きを維持する
    if (delta_x == 0 && delta_y == 0) {
        dir_x_ = 0;
        dir_y_ = 1;
    }

    // 現在の巡回先に到達したら、次の地点へ進む
    if (GetTilePosX() == target.x && GetTilePosY() == target.y) {
        current_index_ = (current_index_ + 1) % patrol_route_.size();
    }
}

void Enemy::Draw() const {

    // 敵本体を円で描画する
    DrawCircle(x_, y_, 12, GetColor(50, 0, 50), TRUE);

    const int half_width = width_ / 2;
    const int half_height = height_ / 2;

    // ===== 進行方向へ向く三角形 =====
    int position_offset_x = -dir_x_ * half_width / 5;
    int position_offset_y = -dir_y_ * half_height / 5;

    int center_x = x_ - dir_x_ * half_width / 2 - position_offset_x;
    int center_y = y_ - dir_y_ * half_height / 2 - position_offset_y;

    int tip_x = center_x + dir_x_ * half_width;
    int tip_y = center_y + dir_y_ * half_height;

    int left_x = center_x - dir_y_ * half_width / 2;
    int left_y = center_y + dir_x_ * half_height / 2;

    int right_x = center_x + dir_y_ * half_width / 2;
    int right_y = center_y - dir_x_ * half_height / 2;

    DrawTriangle(
        tip_x, tip_y,
        left_x, left_y,
        right_x, right_y,
        GetColor(255, 0, 0), FALSE);

    // ===== 後方側の三角形 =====
    // 前後対称の見た目にするため、反対側にも三角形を描く
    position_offset_x = dir_x_ * half_width / 5;
    position_offset_y = dir_y_ * half_height / 5;

    center_x = x_ + dir_x_ * half_width / 2 - position_offset_x;
    center_y = y_ + dir_y_ * half_height / 2 - position_offset_y;

    tip_x = center_x - dir_x_ * half_width;
    tip_y = center_y - dir_y_ * half_height;

    left_x = center_x + dir_y_ * half_width / 2;
    left_y = center_y - dir_x_ * half_height / 2;

    right_x = center_x - dir_y_ * half_width / 2;
    right_y = center_y + dir_x_ * half_height / 2;

    DrawTriangle(
        tip_x, tip_y,
        left_x, left_y,
        right_x, right_y,
        GetColor(255, 0, 0), FALSE);
}