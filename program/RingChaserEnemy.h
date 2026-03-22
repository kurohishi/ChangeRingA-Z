#pragma once

#include "IPosition.h"
#include "Map.h"
#include "Player.h"

// リングを追跡する敵キャラクター
// dist 配列をもとに、リングへ近づく方向へ移動する
class RingChaserEnemy : public IPosition {
public:
    RingChaserEnemy();

    // 開始タイル座標を指定して生成する
    RingChaserEnemy(int start_tile_x, int start_tile_y);

    // 距離マップを参照し、リングへ近づく方向へ更新する
    void Update(const int dist[Map::H][Map::W], const Map& map);

	// プレイヤーと同じタイル上にいるか判定する
    bool CheckHit(const Player& player) const;

    // 敵を描画する
    void Draw() const;

    // ===== IPosition 実装 =====

    // 現在位置（ピクセル座標）を返す
    int GetPixelPosX() const override { return x_; }
    int GetPixelPosY() const override { return y_; }

    // ===== サイズ情報 =====

    int GetWidth() const;
    int GetHeight() const;

    // 現在位置をタイル座標に変換して返す
    int GetTilePosX() const;
    int GetTilePosY() const;

private:

    int x_ = 0;
    int y_ = 0;
    int width_ = 24;
    int height_ = 24;

    // 現在の移動方向
    int dir_x_ = 1;
    int dir_y_ = 1;

    // 生成直後かどうかを表すフラグ
    bool just_spawned_ = true;
};