#include "Ring.h"

#include <cmath>

#include "DxLib.h"
#include "IPosition.h"
#include "Map.h"

namespace {

    // リング再配置時にプレイヤーから最低限離す距離（ピクセル単位）
    constexpr int kMinDistance = 150;
}

Ring::Ring() {
    radius_ = 16;
    x_ = 200;
    y_ = 200;
}

// 現在の配置を指定したタイルへ設定する
void Ring::SetTilePos(int tile_x, int tile_y) {
    x_ = tile_x * Map::TILE + Map::TILE / 2;
    y_ = tile_y * Map::TILE + Map::TILE / 2;
}

// 次回の移動先を指定したタイルへ設定する
void Ring::SetNextTilePos(int tile_x, int tile_y) {
    next_x_ = tile_x * Map::TILE + Map::TILE / 2;
    next_y_ = tile_y * Map::TILE + Map::TILE / 2;
}

// アニメーション用のパラメータを更新する
void Ring::Update() {
    particle_angle_ += particle_speed_;
}

// リング本体と粒子エフェクトを描画する
void Ring::Draw() const {

    // ===== リング本体 =====
    DrawCircle(x_, y_, radius_, GetColor(255, 255, 0), FALSE);
    DrawCircle(x_, y_, radius_ / 2, GetColor(255, 255, 0), FALSE);
    DrawCircle(x_, y_, 4, GetColor(0, 255, 0), FALSE);

    // ===== 周囲の粒子エフェクト =====
    // 半径をわずかに変化させて脈動する見た目にする
    const float base_radius = radius_ - 3 + sinf(particle_angle_ * 2);

    for (int i = 0; i < kParticleCount; ++i) {

        // 円周上に均等配置しつつ回転させる
        const float angle =
            particle_angle_ + (DX_PI_F * 2 / kParticleCount) * i;

        const int particle_x = x_ + cosf(angle) * base_radius;
        const int particle_y = y_ + sinf(angle) * base_radius;

        DrawCircle(particle_x, particle_y, 1, GetColor(100, 255, 255), TRUE);
    }
}

// 対象がリングと同じタイル上にいるかを判定する
bool Ring::CheckHit(const IPosition& target) const {

    // タイル単位で一致しているかを判定 ※ピクセルではなくタイルで判定することで誤差を防ぐ
    const int ring_tile_x = GetTilePosX();
    const int ring_tile_y = GetTilePosY();

    const int target_tile_x = target.GetPixelPosX() / Map::TILE;
    const int target_tile_y = target.GetPixelPosY() / Map::TILE;

    return ring_tile_x == target_tile_x && ring_tile_y == target_tile_y;
}

// リングを別の位置へ再配置する
void Ring::RelocateFar(const IPosition& target, const Map& map) {

    // ===== 事前に指定された移動先がある場合 ===== ※チュートリアルなどで位置を固定したいときに使用
    if (next_x_ != 0 && next_y_ != 0) {
        x_ = next_x_;
        y_ = next_y_;
        return;
    }

    // ===== ランダム再配置 =====
    while (true) {

        // マップ内のランダムなタイルを選択
        const int tile_x = GetRand(Map::W - 1);
        const int tile_y = GetRand(Map::H - 1);

        // タイル中心のピクセル座標に変換
        const int pixel_x = tile_x * Map::TILE + Map::TILE / 2;
        const int pixel_y = tile_y * Map::TILE + Map::TILE / 2;

        // 通行不可ならやり直し
        if (!map.IsWalkable(pixel_x, pixel_y)) {
            continue;
        }

        // プレイヤーとの距離をチェック
        const int delta_x = pixel_x - target.GetPixelPosX();
        const int delta_y = pixel_y - target.GetPixelPosY();

        // 一定距離未満ならやり直し（近すぎる）
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