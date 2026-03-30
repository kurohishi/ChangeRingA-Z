#include "Item.h"

#include <cmath>

#include "DxLib.h"
#include "Map.h"
#include "GameConstants.h"

// タイル座標と種類を指定してアイテムを生成する
Item::Item(int tile_x, int tile_y, ItemType type)
    : GridObject(tile_x, tile_y)
    , type_(type)
{
}

// ランダムな通行可能タイルにアイテムを生成する
Item Item::CreateRandom(const Map& map, ItemType type)
{
    int tile_x = 0;
    int tile_y = 0;

    do {
        tile_x = GetRand(Map::W - 1);
        tile_y = GetRand(Map::H - 1);
    } while (!map.IsWalkableTile(tile_x, tile_y));

    return Item(tile_x, tile_y, type);
}

// アニメーション用の内部状態を更新する
void Item::Update()
{
    ++item_frame_;
}

// プレイヤーと同じタイル上にいるかを判定する
bool Item::CheckHit(const Player& player) const
{
    return IsOnTile(player.GetTilePosX(), player.GetTilePosY());
}

// アイテムを描画する
void Item::Draw() const
{
    // 描画用の中心座標
    const int center_x = x_ + MapConst::kMapTile / 2;
    const int center_y = y_ + MapConst::kMapTile / 2;

    // 色変化アニメーション用の波
    const double time = item_frame_ * ItemConst::kColorWaveSpeed;
    const double wave =
        (std::sin(time) + ItemConst::kColorWaveBase) * ItemConst::kColorWaveScale;

    // アイテムの種類によって描画を変える
    if (type_ == ItemType::SLOW_ENEMY) {
        const int red = static_cast<int>(ItemConst::kBaseColor * wave);
        const int green = ItemConst::kBaseColor;
        const int blue = static_cast<int>(ItemConst::kBaseColor * (1.0 - wave));

        DrawCircle(
            center_x,
            center_y,
            ItemConst::kOuterRadius,
            GetColor(ItemConst::kFrameR, ItemConst::kFrameG, ItemConst::kFrameB),
            TRUE);

        DrawCircle(
            center_x,
            center_y,
            ItemConst::kOuterInnerRadius,
            GetColor(ItemConst::kBlackR, ItemConst::kBlackG, ItemConst::kBlackB),
            TRUE);

        DrawCircle(
            center_x,
            center_y,
            ItemConst::kMiddleRadius,
            GetColor(red, green, blue),
            TRUE);

        DrawCircle(
            center_x,
            center_y,
            ItemConst::kInnerRadius,
            GetColor(ItemConst::kBlackR, ItemConst::kBlackG, ItemConst::kBlackB),
            TRUE);

        DrawCircle(
            center_x,
            center_y,
            ItemConst::kCoreRadius,
            GetColor(ItemConst::kWhiteR, ItemConst::kWhiteG, ItemConst::kWhiteB),
            TRUE);
    }
    else if (type_ == ItemType::BOOST_ALPHABET)
    {
        const int red = static_cast<int>(ItemConst::kBaseColor * wave);
        const int green = static_cast<int>(ItemConst::kBaseColor * (1.0 - wave));
        const int blue = ItemConst::kBaseColor;

        // ===== ひし形サイズ =====
        const int outer = ItemConst::kOuterHalfLength;
        const int inner = ItemConst::kInnerHalfLength;
        const int core = ItemConst::kCoreHalfLength;

        // ===== 外枠（ひし形）=====
        DrawTriangle(center_x, center_y - outer,
            center_x + outer, center_y,
            center_x, center_y + outer,
            GetColor(ItemConst::kFrameR, ItemConst::kFrameG, ItemConst::kFrameB), TRUE);

        DrawTriangle(center_x, center_y - outer,
            center_x - outer, center_y,
            center_x, center_y + outer,
            GetColor(ItemConst::kFrameR, ItemConst::kFrameG, ItemConst::kFrameB), TRUE);

        // ===== 内側（黒）=====
        DrawTriangle(center_x, center_y - (outer - 4),
            center_x + (outer - 4), center_y,
            center_x, center_y + (outer - 4),
            GetColor(ItemConst::kBlackR, ItemConst::kBlackG, ItemConst::kBlackB), TRUE);

        DrawTriangle(center_x, center_y - (outer - 4),
            center_x - (outer - 4), center_y,
            center_x, center_y + (outer - 4),
            GetColor(ItemConst::kBlackR, ItemConst::kBlackG, ItemConst::kBlackB), TRUE);

        // ===== メインカラー（固定色）=====
        DrawTriangle(center_x, center_y - inner,
            center_x + inner, center_y,
            center_x, center_y + inner,
            GetColor(
                ItemConst::kBoostFixedR,
                ItemConst::kBoostFixedG,
                ItemConst::kBoostFixedB),
            TRUE);

        DrawTriangle(center_x, center_y - inner,
            center_x - inner, center_y,
            center_x, center_y + inner,
            GetColor(
                ItemConst::kBoostFixedR,
                ItemConst::kBoostFixedG,
                ItemConst::kBoostFixedB),
            TRUE);

        // ===== コア（色変化）=====
        DrawTriangle(center_x, center_y - core,
            center_x + core, center_y,
            center_x, center_y + core,
            GetColor(red, green, blue), TRUE);

        DrawTriangle(center_x, center_y - core,
            center_x - core, center_y,
            center_x, center_y + core,
            GetColor(red, green, blue), TRUE);
    }
}

// アイテムの種類を返す
ItemType Item::GetType() const
{
    return type_;
}