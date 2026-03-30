#include "DistanceMap.h"

#include <queue>
#include <utility>

#include "GameConstants.h"

namespace DistanceMap
{
	// 指定マスからの距離マップを構築する
    void BuildDistanceMapTile(
        int goal_tile_x,
        int goal_tile_y,
        const Map& map,
        int dist[Map::H][Map::W])
    {
		// 距離マップを初期化する
        for (int y = 0; y < Map::H; ++y) {
            for (int x = 0; x < Map::W; ++x) {
                dist[y][x] = GameConst::kDistUnreachable;
            }
        }

		// リングの位置がマップ外か通行不可タイルなら距離マップは構築できない
        if (goal_tile_x < 0 || goal_tile_y < 0 ||
            goal_tile_x >= Map::W || goal_tile_y >= Map::H) {
            return;
        }

		// リングの位置が通行不可タイルなら距離マップは構築できない
        if (!map.IsWalkableTile(goal_tile_x, goal_tile_y)) {
            return;
        }

		// BFSで距離マップを構築して、ゴールを距離０に設定、qに追加して探索開始
        std::queue<std::pair<int, int>> q;
        dist[goal_tile_y][goal_tile_x] = 0;
        q.push({ goal_tile_x, goal_tile_y });

		// 条件に合うマスを見つけるまで探索する
        while (!q.empty()) {
            auto [x, y] = q.front();
            q.pop();

			// 4方向を探索する
            for (int i = 0; i < GameConst::kBfsDirectionCount; ++i) {
                const int nx = x + RingChaserConst::kDeltaX[i];
                const int ny = y + RingChaserConst::kDeltaY[i];

				// マップ外、通行不可、設定済みのマスなら探索しない
                if (nx < 0 || ny < 0 || nx >= Map::W || ny >= Map::H) {
                    continue;
                }

                if (!map.IsWalkableTile(nx, ny)) {
                    continue;
                }

                if (dist[ny][nx] != GameConst::kDistUnreachable) {
                    continue;
                }

                dist[ny][nx] = dist[y][x] + 1;
                q.push({ nx, ny });
            }
        }
    }
}