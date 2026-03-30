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

    bool CheckHit(const Player& player) const;

    // ===== 座標操作 =====

    void SetTilePos(int tile_x, int tile_y) override;

    // ===== 巡回ルート =====

    void SetPatrolRoute(const std::vector<PatrolPoint>& route);
    bool HasPatrolRoute() const;
    const std::vector<PatrolPoint>& GetPatrolRoute() const;

    // ルート未指定なら、その場で自動生成する
    void EnsureAutoRoute(const Map& map);

private:
    void GenerateAutoRoute(const Map& map);

    bool IsInside(int tile_x, int tile_y) const;
    bool IsWalkableTile(const Map& map, int tile_x, int tile_y) const;
    bool ContainsPoint(
        const std::vector<PatrolPoint>& route,
        int tile_x,
        int tile_y) const;

private:
    std::vector<PatrolPoint> patrol_route_;
    int current_index_ = 0;

    int dir_x_ = 0;
    int dir_y_ = 1;
};