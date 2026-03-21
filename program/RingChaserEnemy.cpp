#include "RingChaserEnemy.h"

#include "DxLib.h"

// ===== RingChaserEnemy =====
RingChaserEnemy::RingChaserEnemy()
    : x_(0), y_(0), just_spawned_(true) {
}
// 開始タイル座標を指定して生成する
RingChaserEnemy::RingChaserEnemy(int start_tile_x, int start_tile_y)
    :x_(start_tile_x* Map::TILE),
     y_(start_tile_y* Map::TILE),
     dir_x_(0),
     dir_y_(1),
     width_(Map::TILE),
     height_(Map::TILE),
     just_spawned_(true) {
}

int RingChaserEnemy::GetWidth() const {
    return width_;
}

int RingChaserEnemy::GetHeight() const {
    return height_;
}

int RingChaserEnemy::GetTilePosX() const {

    return (x_ + Map::TILE / 2) / Map::TILE;
}

int RingChaserEnemy::GetTilePosY() const {

    return (y_ + Map::TILE / 2) / Map::TILE;
}

// 距離マップを参照し、リングへ近づく方向へ更新する
void RingChaserEnemy::Update(const int dist[Map::H][Map::W], const Map& map) 
{
    // 生成直後のフレームは移動しない
    if (just_spawned_) {
        just_spawned_ = false;
        return;
    }

    const int current_tile_x = x_ / Map::TILE;
    const int current_tile_y = y_ / Map::TILE;

    if (current_tile_x < 0 || current_tile_y < 0 ||
        current_tile_x >= Map::W || current_tile_y >= Map::H) {
        return;
    }

    // 到達不能な位置にいる場合は更新しない
    if (dist[current_tile_y][current_tile_x] == -1) {
        return;
    }

    int best_tile_x = current_tile_x;
    int best_tile_y = current_tile_y;
    int best_dist = dist[current_tile_y][current_tile_x];

    // 4方向のうち、よりリングに近づけるマスを探す
    const int delta_x[4] = { 1, -1, 0, 0 };
    const int delta_y[4] = { 0, 0, 1, -1 };

    for (int i = 0; i < 4; ++i) {
        const int next_tile_x = current_tile_x + delta_x[i];
        const int next_tile_y = current_tile_y + delta_y[i];

        if (next_tile_x < 0 || next_tile_y < 0 ||
            next_tile_x >= Map::W || next_tile_y >= Map::H) {
            continue;
        }

        if (!map.IsWalkableTile(next_tile_x, next_tile_y)) {
            continue;
        }

        if (dist[next_tile_y][next_tile_x] == -1) {
            continue;
        }

        // よりリングに近いマスが見つかったら更新する
        if (dist[next_tile_y][next_tile_x] < best_dist) {
            best_dist = dist[next_tile_y][next_tile_x];
            best_tile_x = next_tile_x;
            best_tile_y = next_tile_y;
        }
    }

    // 移動方向を保存して描画に使う
    dir_x_ = best_tile_x - current_tile_x;
    dir_y_ = best_tile_y - current_tile_y;

    // 選んだタイルの左上ピクセル座標へ移動する
    x_ = best_tile_x * Map::TILE;
    y_ = best_tile_y * Map::TILE;
}

// リング追跡敵の描画
void RingChaserEnemy::Draw() const
{
    // ===== 三角形 =====
    const int center_x = x_ + Map::TILE / 2;
    const int center_y = y_ + Map::TILE / 2;

    const int half_width = width_ / 2;
    const int half_height = height_ / 2;

    // 向いている方向を分かりやすくするため、前方に印を描く
    int position_offset_x = -dir_x_ * half_width / 5;
    int position_offset_y = -dir_y_ * half_height / 5;

    int triangle_center_x =
        center_x - dir_x_ * half_width / 2 ;
    int triangle_center_y =
        center_y - dir_y_ * half_height / 2 ;

    int tip_x = triangle_center_x + dir_x_ * half_width;
    int tip_y = triangle_center_y + dir_y_ * half_height;

    int left_x = triangle_center_x - dir_y_ * half_width / 3;
    int left_y = triangle_center_y + dir_x_ * half_height / 3;

    int right_x = triangle_center_x + dir_y_ * half_width / 3;
    int right_y = triangle_center_y - dir_x_ * half_height / 3;

    DrawTriangle(
        tip_x, tip_y,
        left_x, left_y,
        right_x, right_y,
        GetColor(255, 255, 0), FALSE);
}