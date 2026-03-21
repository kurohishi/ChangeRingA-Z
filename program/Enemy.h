#pragma once

#include <vector>

#include "IPosition.h"
#include "Map.h"

// 敵の巡回ルート上の1地点を表す
// x, y はタイル座標として扱う
struct PatrolPoint {
    int x = 0;
    int y = 0;
};

// 巡回ルートに沿って移動する敵キャラクター
// 座標は基本的にピクセル座標で保持し、必要に応じてタイル座標へ変換する
class Enemy : public IPosition {
public:
    // デフォルト構築
    // 主にコンテナ確保や後から設定する用途を想定する
    Enemy() = default;

    // 巡回ルートを指定して敵を生成する
    explicit Enemy(const std::vector<PatrolPoint>& route);

    // 巡回ルートに沿って敵を移動させる
    void Update(const Map& map);

    // 敵を描画する
    void Draw() const;

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

    // 簡易的な当たり判定用半径
    int GetRadius() const { return width_ / 4; }

    // タイル座標を受け取り、そのタイル位置へ敵を配置する
    void SetTilePos(int tile_x, int tile_y);

private:
    // ===== 基本座標（ピクセル座標） =====

    int x_ = 0;
    int y_ = 0;

    // ===== サイズ =====

    int width_ = 24;
    int height_ = 24;

    // ===== 移動方向 =====
    // 巡回ルートへ向かう際の現在の移動方向
    int dir_x_ = 1;
    int dir_y_ = 1;

    // ===== 移動設定 =====

    int speed_ = 0;

    // 巡回ルートの一覧
    // 各地点は PatrolPoint のタイル座標で管理する
    std::vector<PatrolPoint> patrol_route_;

    // 現在向かっている巡回ルート上の地点インデックス
    int current_index_ = 0;
};