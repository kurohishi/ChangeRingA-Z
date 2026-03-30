#include "StageGenerator.h"

#include <algorithm>
#include <cstdlib>
#include <queue>
#include <stack>
#include <utility>
#include <vector>

#include "DxLib.h"

namespace
{
    constexpr int kWall = 0;
    constexpr int kFloor = 1;

    // ループを少し増やす
    constexpr int kExtraConnectionCount = 8;

    // 壁のまとまりを通路化する回数
    constexpr int kExpandRepeatMin = 3;
    constexpr int kExpandRepeatMax = 4;

    // 追加通路どうしの最小距離
    constexpr int kExpandMinDistance = 4;
}

// ===== ステージ自動生成 =====
GeneratedStageData StageGenerator::Generate() const
{
    GeneratedStageData stage{};

    FillWalls(stage);

	// 迷路を作る
    GenerateMaze(stage);

    // ランダムなルートを増やす
    AddExtraConnections(stage, kExtraConnectionCount);

    // 壁の 0,0 / 0,0,0 / 0,0,0,0 を通路化
    ExpandWallSegments(
        stage,
        kExpandRepeatMin,
        kExpandRepeatMax,
        kExpandMinDistance);

    // 3面が壁のマスについて、通路の反対側の壁を掘る
    ExpandDeadEndOppositeWalls(stage);

	// 17、18列目の補正
    CopyColumnsRight(stage);

    DecidePlayerStart(stage);
    DecideRingPos(stage);

    return stage;
}

//全てのマスを壁にする
void StageGenerator::FillWalls(GeneratedStageData& stage) const
{
    for (int y = 0; y < Map::H; ++y)
    {
        for (int x = 0; x < Map::W; ++x) {
            stage.tiles[y][x] = kWall;
        }
    }
}

// 迷路を生成する
void StageGenerator::GenerateMaze(GeneratedStageData& stage) const
{
    bool visited[Map::H][Map::W] = {};

    std::stack<std::pair<int, int>> st;
    st.push({ 1, 1 });
    visited[1][1] = true;
    DigCell(stage, 1, 1);

    const int dx[4] = { 0, 0, 2, -2 };
    const int dy[4] = { 2, -2, 0, 0 };

    while (!st.empty())
    {
        const int x = st.top().first;
        const int y = st.top().second;

        std::vector<int> dirs;

        for (int i = 0; i < 4; ++i)
        {
            const int nx = x + dx[i];
            const int ny = y + dy[i];

            if (nx <= 0 || nx >= Map::W - 1 || ny <= 0 || ny >= Map::H - 1)
            {
                continue;
            }

            if (visited[ny][nx])
            {
                continue;
            }

            dirs.push_back(i);
        }

        if (dirs.empty())
        {
            st.pop();
            continue;
        }

        const int dir = dirs[GetRand(static_cast<int>(dirs.size()) - 1)];
        const int nx = x + dx[dir];
        const int ny = y + dy[dir];

        DigCell(stage, x + dx[dir] / 2, y + dy[dir] / 2);
        DigCell(stage, nx, ny);

        visited[ny][nx] = true;
        st.push({ nx, ny });
    }
}

// 迷路生成後の壁をランダムに掘って、ルートを増やす
void StageGenerator::AddExtraConnections(GeneratedStageData& stage, int count) const
{
    int added = 0;
    int attempt = 0;
    const int kMaxAttempt = 200;

    while (added < count && attempt < kMaxAttempt)
    {
        ++attempt;

        const int x = 1 + GetRand(Map::W - 3);
        const int y = 1 + GetRand(Map::H - 3);

        if (!IsWall(stage, x, y))
        {
            continue;
        }

        const bool up_down =
            IsFloor(stage, x, y - 1) && IsFloor(stage, x, y + 1) &&
            IsWall(stage, x - 1, y) && IsWall(stage, x + 1, y);

        const bool left_right =
            IsFloor(stage, x - 1, y) && IsFloor(stage, x + 1, y) &&
            IsWall(stage, x, y - 1) && IsWall(stage, x, y + 1);

        if (!up_down && !left_right)
        {
            continue;
        }

        DigCell(stage, x, y);
        ++added;
    }
}

// 壁のまとまりを通路化する（小部屋を作る）
void StageGenerator::ExpandWallSegments(
    GeneratedStageData& stage,
    int repeat_min,
    int repeat_max,
    int min_distance) const
{
    std::vector<ExtraCorridor> placed;

    const int repeat_count = repeat_min + GetRand(repeat_max - repeat_min);

    int success_count = 0;
    int attempt = 0;
    const int kMaxAttempt = 500;

	// 長さ2～4の壁をランダムに選んで通路化していく
    while (success_count < repeat_count && attempt < kMaxAttempt)
    {
        ++attempt;

        const int length_roll = GetRand(2);
        int length = 2;
        if (length_roll == 1) length = 3;
        if (length_roll == 2) length = 4;

        const bool horizontal = (GetRand(1) == 0);

        int start_x = 1;
        int start_y = 1;

        if (horizontal)
        {
            start_x = 1 + GetRand(Map::W - 2 - length);
            start_y = 1 + GetRand(Map::H - 3);
        }
        else
        {
            start_x = 1 + GetRand(Map::W - 3);
            start_y = 1 + GetRand(Map::H - 2 - length);
        }

        if (!CanPlaceWallSegment(stage, start_x, start_y, length, horizontal))
        {
            continue;
        }

        if (!IsFarEnough(placed, start_x, start_y, length, horizontal, min_distance))
        {
            continue;
        }

        DigLine(stage, start_x, start_y, length, horizontal);

        ExtraCorridor corridor;
        corridor.x = start_x;
        corridor.y = start_y;
        corridor.length = length;
        corridor.horizontal = horizontal;
        placed.push_back(corridor);

        ++success_count;
    }
}

// 3面が壁のマスについて、通路の反対側の壁を掘る（行き止まりの壁を壊す）
void StageGenerator::ExpandDeadEndOppositeWalls(GeneratedStageData& stage) const
{
    std::vector<std::pair<int, int>> to_dig;

    for (int y = 1; y < Map::H - 1; ++y)
    {
        for (int x = 1; x < Map::W - 1; ++x)
        {
            if (!IsFloor(stage, x, y)) {
                continue;
            }

            const bool up_is_wall = IsWall(stage, x, y - 1);
            const bool down_is_wall = IsWall(stage, x, y + 1);
            const bool left_is_wall = IsWall(stage, x - 1, y);
            const bool right_is_wall = IsWall(stage, x + 1, y);

            const int wall_count =
                static_cast<int>(up_is_wall) +
                static_cast<int>(down_is_wall) +
                static_cast<int>(left_is_wall) +
                static_cast<int>(right_is_wall);

            // 3面が壁のマスだけ対象
            if (wall_count != 3) {
                continue;
            }

            // 各方向が外周壁かどうか
            const bool up_is_outer =
                up_is_wall && IsOuterWallCell(x, y - 1);
            const bool down_is_outer =
                down_is_wall && IsOuterWallCell(x, y + 1);
            const bool left_is_outer =
                left_is_wall && IsOuterWallCell(x - 1, y);
            const bool right_is_outer =
                right_is_wall && IsOuterWallCell(x + 1, y);

            const int outer_wall_count =
                static_cast<int>(up_is_outer) +
                static_cast<int>(down_is_outer) +
                static_cast<int>(left_is_outer) +
                static_cast<int>(right_is_outer);

            int target_x = -1;
            int target_y = -1;

            // 通路の反対側をまず候補にする
            if (!up_is_wall)
            {
                target_x = x;
                target_y = y + 1;
            }
            else if (!down_is_wall)
            {
                target_x = x;
                target_y = y - 1;
            }
            else if (!left_is_wall)
            {
                target_x = x + 1;
                target_y = y;
            }
            else if (!right_is_wall)
            {
                target_x = x - 1;
                target_y = y;
            }

            // 反対側が外周壁でなければ、そのまま掘る
            if (target_x != -1 &&
                target_y != -1 &&
                IsWall(stage, target_x, target_y) &&
                !IsOuterWallCell(target_x, target_y))
            {
                to_dig.push_back({ target_x, target_y });
                continue;
            }

            // 反対側が外周壁 かつ 4方向のうち2面が外周壁なら、外周壁ではない壁を掘る
            if (target_x != -1 &&
                target_y != -1 &&
                IsWall(stage, target_x, target_y) &&
                IsOuterWallCell(target_x, target_y) &&
                outer_wall_count >= 2)
            {
                // 上
                if (up_is_wall && !up_is_outer) {
                    to_dig.push_back({ x, y - 1 });
                    continue;
                }

                // 下
                if (down_is_wall && !down_is_outer) {
                    to_dig.push_back({ x, y + 1 });
                    continue;
                }

                // 左
                if (left_is_wall && !left_is_outer) {
                    to_dig.push_back({ x - 1, y });
                    continue;
                }

                // 右
                if (right_is_wall && !right_is_outer) {
                    to_dig.push_back({ x + 1, y });
                    continue;
                }
            }
        }
    }

    for (const auto& pos : to_dig)
    {
        DigCell(stage, pos.first, pos.second);
    }
}

// 右端の列を、左の列を参考にしてコピーする（17、18列目を整地する）
void StageGenerator::CopyColumnsRight(GeneratedStageData& stage) const
{
    for (int y = 1; y < Map::H - 1; ++y)
    {
        int col16 = stage.tiles[y][Map::W - 4]; // x=16
        int col17 = stage.tiles[y][Map::W - 3]; // x=17

        // ===== 16 → 17（75%でコピー）=====
        if (GetRand(3) != 0) // 0,1,2,3 → 3/4 = 75%
        {
            stage.tiles[y][Map::W - 3] = col16;
        }

        // ===== 17 → 18（100%コピー）=====
        stage.tiles[y][Map::W - 2] = col17;
    }
}

void StageGenerator::DecidePlayerStart(GeneratedStageData& stage) const
{
    const int center_x = Map::W / 2;
    const int center_y = Map::H / 2;

    int best_x = 1;
    int best_y = 1;
    int best_score = -999999;

    for (int y = 1; y < Map::H - 1; ++y)
    {
        for (int x = 1; x < Map::W - 1; ++x)
        {
            if (!IsFloor(stage, x, y))
            {
                continue;
            }

            // 中央からのマンハッタン距離
            const int dist_to_center = std::abs(x - center_x) + std::abs(y - center_y);

            // 周囲4方向の通路数も見る
            int open_count = 0;
            if (IsFloor(stage, x + 1, y)) ++open_count;
            if (IsFloor(stage, x - 1, y)) ++open_count;
            if (IsFloor(stage, x, y + 1)) ++open_count;
            if (IsFloor(stage, x, y - 1)) ++open_count;

            // 中央に近いほど高得点
            // 周囲が少し開けているほど高得点
            const int score = -dist_to_center * 10 + open_count * 3;

            if (score > best_score)
            {
                best_score = score;
                best_x = x;
                best_y = y;
            }
        }
    }

    stage.player_start_x = best_x;
    stage.player_start_y = best_y;
}

void StageGenerator::DecideRingPos(GeneratedStageData& stage) const
{
    const std::vector<std::vector<int>> dist =
        BuildDistanceMap(stage, stage.player_start_x, stage.player_start_y);

    int best_x = stage.player_start_x;
    int best_y = stage.player_start_y;
    int best_dist = -1;

    for (int y = 1; y < Map::H - 1; ++y)
    {
        for (int x = 1; x < Map::W - 1; ++x)
        {
            if (!IsFloor(stage, x, y))
            {
                continue;
            }

            if (dist[y][x] < 0)
            {
                continue;
            }

            if (dist[y][x] > best_dist)
            {
                best_dist = dist[y][x];
                best_x = x;
                best_y = y;
            }
        }
    }

    stage.ring_x = best_x;
    stage.ring_y = best_y;
}

void StageGenerator::DigCell(GeneratedStageData& stage, int x, int y) const
{
    if (!IsInside(x, y))
    {
        return;
    }

    if (x == 0 || x == Map::W - 1 || y == 0 || y == Map::H - 1)
    {
        return;
    }

    stage.tiles[y][x] = kFloor;
}

void StageGenerator::DigLine(
    GeneratedStageData& stage,
    int start_x,
    int start_y,
    int length,
    bool horizontal) const
{
    for (int i = 0; i < length; ++i)
    {
        const int x = horizontal ? (start_x + i) : start_x;
        const int y = horizontal ? start_y : (start_y + i);
        DigCell(stage, x, y);
    }
}

// ===== 自動生成マップ判定用 =====

// 指定したマスがマップ内かどうか
bool StageGenerator::IsInside(int x, int y) const
{
    return x >= 0 && x < Map::W && y >= 0 && y < Map::H;
}

// 指定したマスが通路かどうか
bool StageGenerator::IsFloor(
    const GeneratedStageData& stage,
    int x,
    int y) const
{
    if (!IsInside(x, y))
    {
        return false;
    }

    return stage.tiles[y][x] == kFloor;
}

// 指定したマスが壁かどうか
bool StageGenerator::IsWall(
    const GeneratedStageData& stage,
    int x,
    int y) const
{
    if (!IsInside(x, y))
    {
        return false;
    }

    return stage.tiles[y][x] == kWall;
}

// 指定したマスが外周壁かどうか
bool StageGenerator::IsOuterWallCell(int x, int y) const
{
    return x == 0 || x == Map::W - 1 || y == 0 || y == Map::H - 1;
}

// 指定した長さの壁の塊を置けるかどうか
bool StageGenerator::CanPlaceWallSegment(
    const GeneratedStageData& stage,
    int start_x,
    int start_y,
    int length,
    bool horizontal) const
{
    for (int i = 0; i < length; ++i)
    {
        const int x = horizontal ? (start_x + i) : start_x;
        const int y = horizontal ? start_y : (start_y + i);

        if (!IsInside(x, y))
        {
            return false;
        }

        if (x <= 0 || x >= Map::W - 1 || y <= 0 || y >= Map::H - 1)
        {
            return false;
        }

        if (!IsWall(stage, x, y))
        {
            return false;
        }
    }

    int side_floor_count = 0;

    for (int i = 0; i < length; ++i)
    {
        const int x = horizontal ? (start_x + i) : start_x;
        const int y = horizontal ? start_y : (start_y + i);

        if (horizontal)
        {
            if (IsFloor(stage, x, y - 1)) ++side_floor_count;
            if (IsFloor(stage, x, y + 1)) ++side_floor_count;
        }
        else
        {
            if (IsFloor(stage, x - 1, y)) ++side_floor_count;
            if (IsFloor(stage, x + 1, y)) ++side_floor_count;
        }
    }

    if (side_floor_count > length + 1)
    {
        return false;
    }

    return true;
}

// 既に追加した通路から十分離れているか
bool StageGenerator::IsFarEnough(
    const std::vector<ExtraCorridor>& placed,
    int start_x,
    int start_y,
    int length,
    bool horizontal,
    int min_distance) const
{
    ExtraCorridor candidate;
    candidate.x = start_x;
    candidate.y = start_y;
    candidate.length = length;
    candidate.horizontal = horizontal;

    for (const auto& corridor : placed)
    {
        if (SegmentDistance(candidate, corridor) < min_distance)
        {
            return false;
        }
    }

    return true;
}

// 2つの壁の塊（通路化する前の壁のまとまり）の距離を測る
int StageGenerator::SegmentDistance(
    const ExtraCorridor& a,
    const ExtraCorridor& b) const
{
    const int ax1 = a.x;
    const int ay1 = a.y;
    const int ax2 = a.horizontal ? (a.x + a.length - 1) : a.x;
    const int ay2 = a.horizontal ? a.y : (a.y + a.length - 1);

    const int bx1 = b.x;
    const int by1 = b.y;
    const int bx2 = b.horizontal ? (b.x + b.length - 1) : b.x;
    const int by2 = b.horizontal ? b.y : (b.y + b.length - 1);

    int dx = 0;
    if (ax2 < bx1) dx = bx1 - ax2;
    else if (bx2 < ax1) dx = ax1 - bx2;

    int dy = 0;
    if (ay2 < by1) dy = by1 - ay2;
    else if (by2 < ay1) dy = ay1 - by2;

    return dx + dy;
}

// ステージ内の全ての通路の座標を集める（通路のどこにプレイヤーとリングを置くかを決める）
std::vector<std::pair<int, int>> StageGenerator::CollectFloorTiles(
    const GeneratedStageData& stage) const
{
    std::vector<std::pair<int, int>> result;

    for (int y = 1; y < Map::H - 1; ++y)
    {
        for (int x = 1; x < Map::W - 1; ++x)
        {
            if (IsFloor(stage, x, y))
            {
                result.push_back({ x, y });
            }
        }
    }

    return result;
}

// 指定したマスから各マスへの最短距離を測る
std::vector<std::vector<int>> StageGenerator::BuildDistanceMap(
    const GeneratedStageData& stage,
    int start_x,
    int start_y) const
{
    std::vector<std::vector<int>> dist(
        Map::H,
        std::vector<int>(Map::W, -1));

    if (!IsFloor(stage, start_x, start_y))
    {
        return dist;
    }

    std::queue<std::pair<int, int>> q;
    dist[start_y][start_x] = 0;
    q.push({ start_x, start_y });

    const int dx[4] = { 1, -1, 0, 0 };
    const int dy[4] = { 0, 0, 1, -1 };

    while (!q.empty())
    {
        const int x = q.front().first;
        const int y = q.front().second;
        q.pop();

        for (int i = 0; i < 4; ++i)
        {
            const int nx = x + dx[i];
            const int ny = y + dy[i];

            if (!IsInside(nx, ny))
            {
                continue;
            }

            if (!IsFloor(stage, nx, ny))
            {
                continue;
            }

            if (dist[ny][nx] != -1)
            {
                continue;
            }

            dist[ny][nx] = dist[y][x] + 1;
            q.push({ nx, ny });
        }
    }

    return dist;
}