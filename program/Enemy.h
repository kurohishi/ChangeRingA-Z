#pragma once
#include <vector>

#include "IPosition.h"
#include "Map.h"
#include "Player.h"

// 敵の巡回ルート上の1地点を表す
// x, y はタイル座標として扱う
struct PatrolPoint {
    int x = 0;
    int y = 0;
};

// 巡回ルートに沿って移動する敵
class Enemy : public IPosition {

public:

    Enemy() = default;

    // 巡回ルートを指定して敵を生成する
    explicit Enemy(const std::vector<PatrolPoint>& route);

    // 巡回ルートに沿って敵を移動させる
    void Update(const Map& map);

    // 敵を描画する
    void Draw() const;

	// プレイヤーと同じタイル上にいるか
    bool CheckHit(const Player& player) const;

    // ===== IPosition 実装 =====

    // 現在位置（ピクセル座標）を返す
    int GetPixelPosX() const override { return x_; }
    int GetPixelPosY() const override { return y_; }

    // 現在位置をタイル座標に変換して返す
    int GetTilePosX() const override { return x_ / Map::TILE; }
    int GetTilePosY() const override { return y_ / Map::TILE; }

    // ===== サイズ情報 =====

    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }

    // タイル座標を受け取り、そのタイル位置へ敵を配置する
    void SetTilePos(int tile_x, int tile_y);

private:

	// 位置とサイズ
    int x_ = 0;
    int y_ = 0;
    int width_ = 24;
    int height_ = 24;

    // 移動方向
    int dir_x_ = 1;
    int dir_y_ = 1;
    int speed_ = 0;

    // 巡回ルート ※ 各地点は PatrolPoint のタイル座標で管理する
    std::vector<PatrolPoint> patrol_route_;

    // 現在向かっている巡回ルート上の地点インデックス
    int current_index_ = 0;
};