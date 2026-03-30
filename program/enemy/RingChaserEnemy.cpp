#include "RingChaserEnemy.h"

#include "DxLib.h"
#include "Player.h"
#include "GameConstants.h"

// 開始タイル座標は (0, 0)
RingChaserEnemy::RingChaserEnemy()
    : GridObject(0, 0)
{
}

// 開始タイル座標を指定して生成する
RingChaserEnemy::RingChaserEnemy(int start_tile_x, int start_tile_y)
    : GridObject(start_tile_x, start_tile_y)
{
}

// 距離マップを参照し、リングへ近づく方向へ更新する
void RingChaserEnemy::Update(const int dist[Map::H][Map::W], const Map& map)
{
    // 生成直後のフレームは移動しない
    if (just_spawned_) {
        just_spawned_ = false;
        return;
    }

    const int current_tile_x = x_ / MapConst::kMapTile;
    const int current_tile_y = y_ / MapConst::kMapTile;

    if (current_tile_x < 0 || current_tile_y < 0 ||
        current_tile_x >= Map::W || current_tile_y >= Map::H) {
        return;
    }

    // 到達不能な位置にいる場合は更新しない
    if (dist[current_tile_y][current_tile_x] == GameConst::kDistUnreachable) {
        return;
    }

    int best_tile_x = current_tile_x;
    int best_tile_y = current_tile_y;
    int best_dist = dist[current_tile_y][current_tile_x];

    // 4方向のうち、よりリングに近づけるマスを探す
    for (int i = 0; i < RingChaserConst::kDirectionCount; ++i) {
        const int next_tile_x = current_tile_x + RingChaserConst::kDeltaX[i];
        const int next_tile_y = current_tile_y + RingChaserConst::kDeltaY[i];

        if (next_tile_x < 0 || next_tile_y < 0 ||
            next_tile_x >= Map::W || next_tile_y >= Map::H) {
            continue;
        }

        if (!map.IsWalkableTile(next_tile_x, next_tile_y)) {
            continue;
        }

        if (dist[next_tile_y][next_tile_x] == GameConst::kDistUnreachable) {
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
    x_ = best_tile_x * MapConst::kMapTile;
    y_ = best_tile_y * MapConst::kMapTile;
}

// プレイヤーとのヒット判定
bool RingChaserEnemy::CheckHit(const Player& player) const
{
    return IsOnTile(player.GetTilePosX(), player.GetTilePosY());
}

// リング追跡敵の描画
void RingChaserEnemy::Draw() const
{
    // ===== 三角形 =====
    const int center_x = x_ + MapConst::kMapTile / 2;
    const int center_y = y_ + MapConst::kMapTile / 2;

    const int half_width = RingChaserConst::kWidth / 2;
    const int half_height = RingChaserConst::kHeight / 2;

    // 向いている方向を分かりやすくするため、前方に印を描く
    int position_offset_x = -dir_x_ * half_width / RingChaserConst::kOffsetDiv;
    int position_offset_y = -dir_y_ * half_height / RingChaserConst::kOffsetDiv;

    int triangle_center_x =
        center_x - dir_x_ * half_width / RingChaserConst::kCenterDiv;
    int triangle_center_y =
        center_y - dir_y_ * half_height / RingChaserConst::kCenterDiv;

    // 位置補正を掛けたい場合に備えて残しておく
    triangle_center_x -= position_offset_x;
    triangle_center_y -= position_offset_y;

    int tip_x = triangle_center_x + dir_x_ * half_width;
    int tip_y = triangle_center_y + dir_y_ * half_height;

    int left_x = triangle_center_x - dir_y_ * half_width / RingChaserConst::kTriangleSideDiv;
    int left_y = triangle_center_y + dir_x_ * half_height / RingChaserConst::kTriangleSideDiv;

    int right_x = triangle_center_x + dir_y_ * half_width / RingChaserConst::kTriangleSideDiv;
    int right_y = triangle_center_y - dir_x_ * half_height / RingChaserConst::kTriangleSideDiv;

    DrawTriangle(
        tip_x, tip_y,
        left_x, left_y,
        right_x, right_y,
        GetColor(
            RingChaserConst::kTriangleColorR,
            RingChaserConst::kTriangleColorG,
            RingChaserConst::kTriangleColorB),
        FALSE);
}