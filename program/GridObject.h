#pragma once

#include "IPosition.h"
#include "Map.h"

// タイル座標とピクセル座標を持つ共通基底クラス
class GridObject : public IPosition
{
public:
    GridObject() = default;

    GridObject(int tile_x, int tile_y)
    {
        SetTilePos(tile_x, tile_y);
    }

    // ===== IPosition =====

    int GetPixelPosX() const override
    {
        return x_;
    }

    int GetPixelPosY() const override
    {
        return y_;
    }

    int GetTilePosX() const override
    {
        return x_ / Map::TILE;
    }

    int GetTilePosY() const override
    {
        return y_ / Map::TILE;
    }

    // ===== 座標設定 =====

    virtual void SetPixelPos(int x, int y)
    {
        x_ = x;
        y_ = y;
    }

    virtual void SetTilePos(int tile_x, int tile_y)
    {
        x_ = tile_x * Map::TILE;
        y_ = tile_y * Map::TILE;
    }

    virtual void SetTileCenterPos(int tile_x, int tile_y)
    {
        x_ = tile_x * Map::TILE + Map::TILE / 2;
        y_ = tile_y * Map::TILE + Map::TILE / 2;
    }

    // ===== 補助判定 =====

    bool IsOnTile(int tile_x, int tile_y) const
    {
        return GetTilePosX() == tile_x &&
            GetTilePosY() == tile_y;
    }

protected:
    int x_ = 0;
    int y_ = 0;
};