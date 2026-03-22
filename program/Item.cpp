#include "Item.h"

#include <cmath>

#include "DxLib.h"
#include "Map.h"

Item::Item(int tile_x, int tile_y, ItemType type)
    : type_(type) {
    x_ = tile_x * Map::TILE;
    y_ = tile_y * Map::TILE;
}

Item Item::CreateRandom(const Map& map, ItemType type)
{
    int posX = 0;
    int posY = 0;

    do {
        posX = GetRand(Map::W - 1);
        posY = GetRand(Map::H - 1);
    } while (!map.IsWalkableTile(posX, posY));

    return Item(posX, posY, type);
}

void Item::Update() {
    // 演出用パラメータを進める
    anim_ += 0.05f;
    ++item_frame_;
}

bool Item::CheckHit(const Player& player) const
{
    return player.GetTilePosX() == GetTilePosX() &&
        player.GetTilePosY() == GetTilePosY();
}

void Item::Draw() const {

    // タイル座標を描画用の中心ピクセル座標へ変換する
    const int center_x = x_ + Map::TILE / 2;
    const int center_y = y_ + Map::TILE / 2;

    // フレーム数をもとに色がゆっくり変化するアニメーションを作る
    const double time = item_frame_ * 0.05;
    const double wave = (std::sin(time) + 1.0) * 0.5;

    if (type_ == ItemType::SLOW_ENEMY)
    {
        // 色を水色?赤で変化させる
        const int red = static_cast<int>(200 * wave);
        const int green = 200;
        const int blue = static_cast<int>(200 * (1.0 - wave));

        DrawCircle(center_x, center_y, 11, GetColor(200, 180, 255), TRUE);
        DrawCircle(center_x, center_y, 10, GetColor(0, 0, 0), TRUE);
        DrawCircle(center_x, center_y, 8, GetColor(red, green, blue), TRUE);
        DrawCircle(center_x, center_y, 6, GetColor(0, 0, 0), TRUE);
        DrawCircle(center_x, center_y, 4, GetColor(255, 255, 255), TRUE);
    }
    else if (type_ == ItemType::BOOST_ALPHABET)
    {
		// 色を赤?青で変化させる
        const int red = static_cast<int>(200 * wave);
        const int green = static_cast<int>(200 * (1.0 - wave));
        const int blue = 200;

        DrawCircle(center_x, center_y, 11, GetColor(200, 180, 255), TRUE);
        DrawCircle(center_x, center_y, 10, GetColor(0, 0, 0), TRUE);
        DrawCircle(center_x, center_y, 8, GetColor(255, 255, 0), TRUE);
        DrawCircle(center_x, center_y, 6, GetColor(0, 0, 0), TRUE);
        DrawCircle(center_x, center_y, 4, GetColor(red, green, blue), TRUE);
    }
}

ItemType Item::GetType() const {
    return type_;
}