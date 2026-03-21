#include "Portal.h"

#include <cmath>

#include "DxLib.h"
#include "Map.h"

Portal::Portal(int tile_x, int tile_y, int pair_index)
    : x_(tile_x), y_(tile_y), pair_index_(pair_index) {
}

void Portal::SetTilePos(int tile_x, int tile_y) {

    // ポータル位置はタイル座標で保持する
    x_ = tile_x;
    y_ = tile_y;
}

void Portal::Update() {

    // 0.0f ～ 1.0f の範囲でループするアニメーション値
    anim_ += 0.01f;

    if (anim_ >= 1.0f) {
        anim_ = 0.0f;
    }
}

void Portal::DrawPortalBoxWave(float scale) const {

    // タイル座標を描画用のピクセル座標へ変換する
    const int size = static_cast<int>(Map::TILE * scale);
    const int offset = (Map::TILE - size) / 2;

    DrawBox(
        x_ * Map::TILE + offset,
        y_ * Map::TILE + offset,
        x_ * Map::TILE + offset + size,
        y_ * Map::TILE + offset + size,
        GetColor(0, 255, 255),
        FALSE);
}

void Portal::Draw() {

    // 位相をずらした四角形を 2 つ重ねて収縮するアニメーション
    const float wave_1 = anim_;
    float wave_2 = anim_ + 0.5f;

    if (wave_2 >= 1.0f) {
        wave_2 -= 1.0f;
    }

    const float scale_1 = 1.0f - wave_1;
    const float scale_2 = 1.0f - wave_2;

    DrawPortalBoxWave(scale_1);
    DrawPortalBoxWave(scale_2);
}

bool Portal::CheckHit(const Player& player) const {

    // プレイヤーと同じタイル上にいるかで判定する
    return player.GetTilePosX() == x_ &&
        player.GetTilePosY() == y_;
}

int Portal::GetTileX() const {
    return x_;
}

int Portal::GetTileY() const {
    return y_;
}

int Portal::GetPairIndex() const {
    return pair_index_;
}