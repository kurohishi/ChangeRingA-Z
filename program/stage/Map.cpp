#include "Map.h"

#include "DxLib.h"
#include "GameConstants.h"

namespace
{
    // ===== チュートリアル用マップ =====

    const int kMapTutorialA[Map::H][Map::W] =
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    const int kMapTutorialB[Map::H][Map::W] =
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    const int kMapTutorialC[Map::H][Map::W] =
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
}  // namespace

// ===== マップ生成 =====
Map::Map()
{
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            data_[y][x] = MapConst::kWall;
        }
    }
}

// ===== チュートリアル用マップ =====

// リング＆ワープポータルのチュートリアルマップ
void Map::UseTutorialMapA()
{
    SetMapData(kMapTutorialA);
}

// 巡回者のチュートリアルマップ
void Map::UseTutorialMapB()
{
    SetMapData(kMapTutorialB);
}

//追跡者＆加速アイテムのチュートリアルマップ
void Map::UseTutorialMapC()
{
    SetMapData(kMapTutorialC);
}

// 全マスを通路にする
void Map::UseAllRoadMap()
{
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            data_[y][x] = MapConst::kFloor;
        }
    }
}

// 外部マップデータを現在のマップへコピーする
void Map::SetMapData(const int src[H][W])
{
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            data_[y][x] = src[y][x];
        }
    }
}

// 裏世界状態を設定する
void Map::SetAltWorld(bool is_alt_world)
{
    is_alt_world_ = is_alt_world;
}

// 現在のマップを描画する
void Map::Draw() const
{
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            int color = 0;

            if (data_[y][x] == MapConst::kWall) {
                // 壁
                color = is_alt_world_
                    ? GetColor(
                        MapConst::kAltWallColorR,
                        MapConst::kAltWallColorG,
                        MapConst::kAltWallColorB)
                    : GetColor(
                        MapConst::kWallColorR,
                        MapConst::kWallColorG,
                        MapConst::kWallColorB);
            }
            else {
                // 通路
                color = is_alt_world_
                    ? GetColor(
                        MapConst::kAltFloorColorR,
                        MapConst::kAltFloorColorG,
                        MapConst::kAltFloorColorB)
                    : GetColor(
                        MapConst::kFloorColorR,
                        MapConst::kFloorColorG,
                        MapConst::kFloorColorB);
            }

            DrawBox(
                x * TILE,
                y * TILE,
                x * TILE + TILE,
                y * TILE + TILE,
                color,
                TRUE);
        }
    }

    // マスの境界を見やすくするためのグリッド線
    const int line_color = GetColor(
        MapConst::kGridColorR,
        MapConst::kGridColorG,
        MapConst::kGridColorB);

    for (int x = 0; x <= W; ++x) {
        const int pixel_x = x * TILE;
        DrawLine(pixel_x, 0, pixel_x, H * TILE, line_color);
    }

    for (int y = 0; y <= H; ++y) {
        const int pixel_y = y * TILE;
        DrawLine(0, pixel_y, W * TILE, pixel_y, line_color);
    }
}

// 指定したピクセル座標が通行可能か
bool Map::IsWalkable(int pixel_x, int pixel_y) const
{
    const int tile_x = pixel_x / TILE;
    const int tile_y = pixel_y / TILE;

    if (tile_x < 0 || tile_y < 0 || tile_x >= W || tile_y >= H) {
        return false;
    }

    return data_[tile_y][tile_x] == MapConst::kFloor;
}

// 指定した範囲が通行可能か
bool Map::IsWalkableRect(int pixel_x, int pixel_y, int width, int height) const
{
    // 四隅がすべて通行可能なら、この矩形は移動可能とみなす
    const int left = pixel_x;
    const int right = pixel_x + width - 1;
    const int top = pixel_y;
    const int bottom = pixel_y + height - 1;

    const int tile_x1 = left / TILE;
    const int tile_y1 = top / TILE;
    const int tile_x2 = right / TILE;
    const int tile_y2 = bottom / TILE;

    if (tile_x1 < 0 || tile_y1 < 0 || tile_x2 >= W || tile_y2 >= H) {
        return false;
    }

    return data_[tile_y1][tile_x1] == MapConst::kFloor &&
        data_[tile_y1][tile_x2] == MapConst::kFloor &&
        data_[tile_y2][tile_x1] == MapConst::kFloor &&
        data_[tile_y2][tile_x2] == MapConst::kFloor;
}

// 指定したマスが通行可能か
bool Map::IsWalkableTile(int tile_x, int tile_y) const
{
    if (tile_x < 0 || tile_y < 0 || tile_x >= W || tile_y >= H) {
        return false;
    }

    return data_[tile_y][tile_x] == MapConst::kFloor;
}

// 最初に見つかった通行可能マスの位置を返す
void Map::GetFirstWalkablePos(int& out_pixel_x, int& out_pixel_y) const
{
    // 左上から順に探し、最初の通路を返す
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            if (data_[y][x] == MapConst::kFloor) {
                out_pixel_x = x * TILE;
                out_pixel_y = y * TILE;
                return;
            }
        }
    }

    // 万が一通路がない場合の保険
    out_pixel_x = MapConst::kFallbackTileX * TILE;
    out_pixel_y = MapConst::kFallbackTileY * TILE;
}