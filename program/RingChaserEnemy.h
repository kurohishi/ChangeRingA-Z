#pragma once

#include "GridObject.h"
#include "Map.h"
#include "Player.h"

// リングを追跡する敵クラス
// 距離マップを参照し、リングに近づく方向へ移動する
class RingChaserEnemy : public GridObject
{
public:
    // ===== 生成 =====

    RingChaserEnemy();
    RingChaserEnemy(int start_tile_x, int start_tile_y);

    // ===== 更新・描画 =====

    // 距離マップを参照して移動を更新する
    void Update(const int dist[Map::H][Map::W], const Map& map);

    // 敵を描画する
    void Draw() const;

    // ===== 判定 =====

    // プレイヤーと同じタイルにいるかを判定する
    bool CheckHit(const Player& player) const;

    // ===== サイズ取得 =====

    int GetWidth() const;
    int GetHeight() const;

private:
    // ===== 基本情報 =====

    int width_ = RingChaserConst::kWidth;
    int height_ = RingChaserConst::kHeight;

    // ===== 移動関連 =====

    // 現在の移動方向
    int dir_x_ = RingChaserConst::kInitialDirX;
    int dir_y_ = RingChaserConst::kInitialDirY;

    // ===== 状態 =====

    // 生成直後かどうか
    bool just_spawned_ = true;
};