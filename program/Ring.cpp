#include "Ring.h"

#include <cmath>

#include "DxLib.h"
#include "IPosition.h"
#include "Map.h"
#include "Constants.h"

namespace
{
    // リング再配置時に対象から最低限離す距離（ピクセル単位）
    constexpr int kMinDistance = RingConst::kMinRelocateDistance;
}

// ===== Ring =====

Ring::Ring()
{
    radius_ = RingConst::kRadius;
    x_ = RingConst::kStartX;
    y_ = RingConst::kStartY;
}

// 現在位置を指定タイルの中心に設定する
void Ring::SetTilePos(int tile_x, int tile_y)
{
    x_ = tile_x * Map::TILE + Map::TILE / 2;
    y_ = tile_y * Map::TILE + Map::TILE / 2;
}

// 次回移動先を指定タイルの中心として予約する
void Ring::SetNextTilePos(int tile_x, int tile_y)
{
    next_x_ = tile_x * Map::TILE + Map::TILE / 2;
    next_y_ = tile_y * Map::TILE + Map::TILE / 2;
}

// 次回移動先の予約をクリアする
void Ring::ClearNextPos()
{
    next_x_ = RingConst::kNoNextPosition;
    next_y_ = RingConst::kNoNextPosition;
}

// アニメーション用パラメータを更新する
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

    // ===== 周囲の粒子エフェクト =====
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

// 対象がリングと同じタイル上にいるかを判定する
bool Ring::CheckHit(const IPosition& target) const
{
    // ピクセル単位ではなくタイル単位で判定して誤差を防ぐ
    const int ring_tile_x = GetTilePosX();
    const int ring_tile_y = GetTilePosY();

    const int target_tile_x = target.GetPixelPosX() / Map::TILE;
    const int target_tile_y = target.GetPixelPosY() / Map::TILE;

    return ring_tile_x == target_tile_x &&
        ring_tile_y == target_tile_y;
}

// リングを別の位置へ再配置する
void Ring::RelocateFar(const IPosition& target, const Map& map)
{
    // ===== 予約済みの移動先がある場合 =====
    // チュートリアルなどで位置を固定したいときに使う
    if (next_x_ != RingConst::kNoNextPosition &&
        next_y_ != RingConst::kNoNextPosition) {
        x_ = next_x_;
        y_ = next_y_;
        return;
    }

    // ===== ランダム再配置 =====
    while (true) {
        // マップ内のランダムなタイルを選ぶ
        const int tile_x = GetRand(Map::W - 1);
        const int tile_y = GetRand(Map::H - 1);

        // タイル中心のピクセル座標へ変換
        const int pixel_x = tile_x * Map::TILE + Map::TILE / 2;
        const int pixel_y = tile_y * Map::TILE + Map::TILE / 2;

        // 通行不可ならやり直し
        if (!map.IsWalkable(pixel_x, pixel_y)) {
            continue;
        }

        // 対象との距離を確認
        const int delta_x = pixel_x - target.GetPixelPosX();
        const int delta_y = pixel_y - target.GetPixelPosY();

        // 近すぎる場合はやり直し
        if (delta_x * delta_x + delta_y * delta_y <
            kMinDistance * kMinDistance) {
            continue;
        }

        // 条件を満たしたので確定
        x_ = pixel_x;
        y_ = pixel_y;
        return;
    }
}