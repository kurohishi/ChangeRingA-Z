#include "Portal.h"

#include <cmath>

#include "DxLib.h"
#include "Map.h"
#include "Constants.h"

// ===== Portal =====

// タイル座標と接続先ペア番号を指定して生成する
Portal::Portal(int tile_x, int tile_y, int pair_index)
    : GridObject(tile_x, tile_y)
    , pair_index_(pair_index)
{
}

// タイル座標で配置位置を設定する
void Portal::SetTilePos(int tile_x, int tile_y)
{
    GridObject::SetTilePos(tile_x, tile_y);
}

// アニメーション用の内部状態を更新する
void Portal::Update()
{
    // 0.0f ～ 1.0f の範囲でループする値
    anim_ += PortalConst::kAnimSpeed;

    if (anim_ >= PortalConst::kAnimLoopMax) {
        anim_ = 0.0f;
    }
}

// ポータル枠を拡縮率に応じて描画する
void Portal::DrawPortalBoxWave(float scale) const
{
    const int size = static_cast<int>(Map::TILE * scale);
    const int offset = (Map::TILE - size) / 2;

    DrawBox(
        x_ + offset,
        y_ + offset,
        x_ + offset + size,
        y_ + offset + size,
        GetColor(
            PortalConst::kColorR,
            PortalConst::kColorG,
            PortalConst::kColorB),
        FALSE);
}

// ポータルを描画する
void Portal::Draw() const
{
    // 位相をずらした2つの四角形を重ねて波打つように見せる
    const float wave_1 = anim_;
    float wave_2 = anim_ + PortalConst::kSecondWaveOffset;

    if (wave_2 >= PortalConst::kAnimLoopMax) {
        wave_2 -= PortalConst::kAnimLoopMax;
    }

    const float scale_1 = PortalConst::kAnimLoopMax - wave_1;
    const float scale_2 = PortalConst::kAnimLoopMax - wave_2;

    DrawPortalBoxWave(scale_1);
    DrawPortalBoxWave(scale_2);
}

// プレイヤーと同じタイルにいるかを判定する
bool Portal::CheckHit(const Player& player) const
{
    return IsOnTile(player.GetTilePosX(), player.GetTilePosY());
}

// 接続先ペア番号を返す
int Portal::GetPairIndex() const
{
    return pair_index_;
}