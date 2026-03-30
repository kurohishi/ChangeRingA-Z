#include "Ring.h"

#include <cmath>

#include "DxLib.h"
#include "IPosition.h"
#include "Map.h"
#include "GameConstants.h"

namespace
{
    // リング再配置時に対象から最低限離す距離（ピクセル単位）
    constexpr int kMinDistance = RingConst::kMinRelocateDistance;
}

// リングを生成する
Ring::Ring()
{
    radius_ = RingConst::kRadius;
    x_ = RingConst::kStartX;
    y_ = RingConst::kStartY;
}

// 現在位置を指定タイルの中心に設定する
void Ring::SetTilePos(int tile_x, int tile_y)
{
    x_ = tile_x * MapConst::kMapTile + MapConst::kMapTile / 2;
    y_ = tile_y * MapConst::kMapTile + MapConst::kMapTile / 2;
}

// 次回移動先を指定タイルの中心として設定する
void Ring::SetNextTilePos(int tile_x, int tile_y)
{
    next_x_ = tile_x * MapConst::kMapTile + MapConst::kMapTile / 2;
    next_y_ = tile_y * MapConst::kMapTile + MapConst::kMapTile / 2;
}

// 次回移動先の設定をクリアする
void Ring::ClearNextPos()
{
    next_x_ = RingConst::kNoNextPosition;
    next_y_ = RingConst::kNoNextPosition;
}

// 粒子アニメーションを更新する
void Ring::Update()
{
    particle_angle_ += particle_speed_;
}

// リング本体と粒子エフェクトを描画する
void Ring::Draw() const
{
    // ===== リング本体 =====
    DrawCircle(
        x_,
        y_,
        radius_,
        GetColor(
            RingConst::kRingColorR,
            RingConst::kRingColorG,
            RingConst::kRingColorB),
        FALSE);

    DrawCircle(
        x_,
        y_,
        radius_ / RingConst::kInnerRingRadiusDiv,
        GetColor(
            RingConst::kRingColorR,
            RingConst::kRingColorG,
            RingConst::kRingColorB),
        FALSE);

    DrawCircle(
        x_,
        y_,
        RingConst::kCoreRadius,
        GetColor(
            RingConst::kCoreColorR,
            RingConst::kCoreColorG,
            RingConst::kCoreColorB),
        FALSE);

    // ===== 粒子エフェクト =====
    // 半径を少し変化させて脈動する見た目にする
    const float base_radius =
        radius_ - RingConst::kPulseOffset +
        sinf(particle_angle_ * RingConst::kPulseSpeed);

    for (int i = 0; i < kParticleCount; ++i) {
        // 円周上に均等配置しつつ回転させる
        const float angle =
            particle_angle_ + (DX_PI_F * 2 / kParticleCount) * i;

        const int particle_x = x_ + static_cast<int>(cosf(angle) * base_radius);
        const int particle_y = y_ + static_cast<int>(sinf(angle) * base_radius);

        DrawCircle(
            particle_x,
            particle_y,
            RingConst::kParticleRadius,
            GetColor(
                RingConst::kParticleColorR,
                RingConst::kParticleColorG,
                RingConst::kParticleColorB),
            TRUE);
    }
}

// 対象がリングと同じタイル上にいるか
bool Ring::CheckHit(const IPosition& target) const
{
    const int ring_tile_x = GetTilePosX();
    const int ring_tile_y = GetTilePosY();

    const int target_tile_x = target.GetPixelPosX() / MapConst::kMapTile;
    const int target_tile_y = target.GetPixelPosY() / MapConst::kMapTile;

    return ring_tile_x == target_tile_x &&
        ring_tile_y == target_tile_y;
}

// リングを別の位置へ再配置する
void Ring::RelocateFar(const IPosition& target, const Map& map)
{
    // ===== 設定済みの移動先がある場合 =====

    if (next_x_ != RingConst::kNoNextPosition &&
        next_y_ != RingConst::kNoNextPosition) {
        x_ = next_x_;
        y_ = next_y_;
        return;
    }

    // ===== ランダム再配置 =====
    while (true) {
        const int tile_x = GetRand(Map::W - 1);
        const int tile_y = GetRand(Map::H - 1);

        const int pixel_x = tile_x * MapConst::kMapTile + MapConst::kMapTile / 2;
        const int pixel_y = tile_y * MapConst::kMapTile + MapConst::kMapTile / 2;

        if (!map.IsWalkable(pixel_x, pixel_y)) {
            continue;
        }

        const int delta_x = pixel_x - target.GetPixelPosX();
        const int delta_y = pixel_y - target.GetPixelPosY();

        if (delta_x * delta_x + delta_y * delta_y <
            kMinDistance * kMinDistance) {
            continue;
        }

        x_ = pixel_x;
        y_ = pixel_y;
        return;
    }
}