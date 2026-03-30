#pragma once

#include <utility>
#include <vector>

#include "Map.h"

// ===== 追加通路情報 =====
struct ExtraCorridor
{
    int x = 0;
    int y = 0;
    int length = 0;
    bool horizontal = true;
};

// ===== 自動生成ステージデータ =====
struct GeneratedStageData
{
    int tiles[Map::H][Map::W] = {};
    int player_start_x = 1;
    int player_start_y = 1;
    int ring_x = 1;
    int ring_y = 1;
};

// ===== ステージ自動生成クラス =====
class StageGenerator
{
public:
    GeneratedStageData Generate() const;

private:
    void FillWalls(GeneratedStageData& stage) const;
    void GenerateMaze(GeneratedStageData& stage) const;
    void AddExtraConnections(GeneratedStageData& stage, int count) const;
    void ExpandWallSegments(
        GeneratedStageData& stage,
        int repeat_min,
        int repeat_max,
        int min_distance) const;
    void ExpandDeadEndOppositeWalls(GeneratedStageData& stage) const;
    void CopyColumnsRight(GeneratedStageData& stage) const;

    void DecidePlayerStart(GeneratedStageData& stage) const;
    void DecideRingPos(GeneratedStageData& stage) const;

    void DigCell(GeneratedStageData& stage, int x, int y) const;
    void DigLine(
        GeneratedStageData& stage,
        int start_x,
        int start_y,
        int length,
        bool horizontal) const;

    bool IsInside(int x, int y) const;
    bool IsFloor(
        const GeneratedStageData& stage,
        int x,
        int y) const;
    bool IsWall(
        const GeneratedStageData& stage,
        int x,
        int y) const;
    bool IsOuterWallCell(int x, int y) const;

    bool CanPlaceWallSegment(
        const GeneratedStageData& stage,
        int start_x,
        int start_y,
        int length,
        bool horizontal) const;
    bool IsFarEnough(
        const std::vector<ExtraCorridor>& placed,
        int start_x,
        int start_y,
        int length,
        bool horizontal,
        int min_distance) const;
    int SegmentDistance(
        const ExtraCorridor& a,
        const ExtraCorridor& b) const;

    std::vector<std::pair<int, int>> CollectFloorTiles(
        const GeneratedStageData& stage) const;
    std::vector<std::vector<int>> BuildDistanceMap(
        const GeneratedStageData& stage,
        int start_x,
        int start_y) const;
};