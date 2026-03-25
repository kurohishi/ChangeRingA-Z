#pragma once

#include <vector>

#include "GridObject.h"
#include "Map.h"
#include "Player.h"

// 巡回ルート上の1マス分の地点（タイル座標）
struct PatrolPoint
{
    int x = 0;
    int y = 0;
};

// 巡回ルートに沿って移動する敵クラス
class PatrolEnemy : public GridObject
{
public:
    // ===== 生成 =====

    PatrolEnemy() = default;
    explicit PatrolEnemy(const std::vector<PatrolPoint>& route);

    // ===== 更新・描画 =====

    void Update(const Map& map);
    void Draw() const;

    // ===== 当たり判定 =====

    // プレイヤーと同じタイルにいるかを判定
    bool CheckHit(const Player& player) const;

    // ===== 座標操作 =====

    // 指定したタイル座標の位置に敵を配置する
    void SetTilePos(int tile_x, int tile_y) override;

    // ===== サイズ取得 =====

    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }

private:

    // ===== サイズ =====

    int width_ = PatrolEnemyConst::kWidth;
    int height_ = PatrolEnemyConst::kHeight;

    // ===== 移動設定 =====

    int dir_x_ = PatrolEnemyConst::kInitialDirX;
    int dir_y_ = PatrolEnemyConst::kInitialDirY;
    int speed_ = 0;

    // ===== 巡回ルート情報 =====

    // 巡回ルートはタイル座標で保持
    std::vector<PatrolPoint> patrol_route_;

    // 現在向かっている巡回先のインデックス
    int current_index_ = 0;
};