#pragma once

#include "Map.h"

namespace DistanceMap
{
    void BuildDistanceMapTile(
        int goal_tile_x,
        int goal_tile_y,
        const Map& map,
        int dist[Map::H][Map::W]);
}