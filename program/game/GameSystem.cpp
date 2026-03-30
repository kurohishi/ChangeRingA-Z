#include "Game.h"

#include <queue>
#include <cmath>

#include "DxLib.h"

#include "GameConstants.h"
#include "SoundID.h"
#include "SoundManager.h"
#include "StageGenerator.h"
#include "SignalManager.h"
#include "DistanceMap.h"

// 生成されたステージの初期化
void Game::SetupGeneratedStage()
{
    StageGenerator generator;

    generated_stage_a_ = generator.Generate();
    generated_stage_b_ = generator.Generate();
    has_generated_stage_pair_ = true;

    is_alt_world_ = false;
    ApplyCurrentGeneratedMap();
    map_.SetAltWorld(false);

    player_.SetTilePos(
        generated_stage_a_.player_start_x,
        generated_stage_a_.player_start_y);

    ring_.SetTilePos(
        generated_stage_a_.ring_x,
        generated_stage_a_.ring_y);
    ring_.ClearNextPos();

    enemies_a_.clear();
    enemies_b_.clear();
    chasers_.clear();
    portals_.clear();
    items_.clear();

    current_enemies_ = &enemies_a_;

    alphabet_boost_count_ = 0;
    enemy_move_delay_ = GameConst::kGameStartEnemyMoveDelay;
    next_enemy_move_delay_ = GameConst::kGameStartNextEnemyMoveDelay;
    player_move_count_ = 0;
    delay_item_timer_ = 0;
    player_on_portal_ = false;
    portal_cooldown_ = 0;
    is_warping_ = false;
    clear_time_ = 0.0;
}

// 生成されたステージのマップデータをマップオブジェクトに適用する
void Game::ApplyCurrentGeneratedMap()
{
    if (!has_generated_stage_pair_) {
        return;
    }

    if (is_alt_world_) {
        map_.SetMapData(generated_stage_b_.tiles);
    }
    else {
        map_.SetMapData(generated_stage_a_.tiles);
    }
}

// 生成されたステージの指定タイルが通行可能か
bool Game::IsGeneratedStageWalkable(
    const GeneratedStageData& stage,
    int tile_x,
    int tile_y) const
{
    if (tile_x < 0 || tile_y < 0 ||
        tile_x >= Map::W || tile_y >= Map::H) {
        return false;
    }

    return stage.tiles[tile_y][tile_x] != 0;
}

// リングの位置が両方のステージで通行可能か
bool Game::CanSwitchCurrentRingPosBetweenGeneratedStages() const
{
    if (!has_generated_stage_pair_) {
        return false;
    }

    const int ring_x = ring_.GetTilePosX();
    const int ring_y = ring_.GetTilePosY();

    return IsGeneratedStageWalkable(generated_stage_a_, ring_x, ring_y) &&
        IsGeneratedStageWalkable(generated_stage_b_, ring_x, ring_y);
}

// 生成されたステージデータからマップデータを構築する
Map Game::BuildGeneratedStageMap(
    const GeneratedStageData& stage,
    bool is_alt_world) const
{
    Map temp_map;
    temp_map.SetMapData(stage.tiles);
    temp_map.SetAltWorld(is_alt_world);
    return temp_map;
}

// 直線距離を計算する
int Game::GetManhattanDistance(int x1, int y1, int x2, int y2) const
{
    return static_cast<int>(std::abs(x1 - x2) + std::abs(y1 - y2));
}

// 指定マスにプレイヤーやリング、敵などのオブジェクトがいるか
bool Game::IsOccupiedTile(int tile_x, int tile_y) const
{
    if (player_.GetTilePosX() == tile_x &&
        player_.GetTilePosY() == tile_y) {
        return true;
    }

    if (ring_.GetTilePosX() == tile_x &&
        ring_.GetTilePosY() == tile_y) {
        return true;
    }

    for (const auto& portal : portals_) {
        if (portal.GetTilePosX() == tile_x &&
            portal.GetTilePosY() == tile_y) {
            return true;
        }
    }

    for (const auto& item : items_) {
        if (item.GetTilePosX() == tile_x &&
            item.GetTilePosY() == tile_y) {
            return true;
        }
    }

    for (const auto& chaser : chasers_) {
        if (chaser.GetTilePosX() == tile_x &&
            chaser.GetTilePosY() == tile_y) {
            return true;
        }
    }

    if (current_enemies_ != nullptr) {
        for (const auto& enemy : *current_enemies_) {
            if (enemy.GetTilePosX() == tile_x &&
                enemy.GetTilePosY() == tile_y) {
                return true;
            }
        }
    }

    return false;
}

// 指定マスにオブジェクトを配置できるか
bool Game::CanPlaceObjectTile(int tile_x, int tile_y) const
{
    if (tile_x <= 0 || tile_y <= 0 ||
        tile_x >= Map::W - 1 || tile_y >= Map::H - 1) {
        return false;
    }

    if (!map_.IsWalkableTile(tile_x, tile_y)) {
        return false;
    }

    if (IsOccupiedTile(tile_x, tile_y)) {
        return false;
    }

    return true;
}

// 指定マスが広い通路の一部か（上下左右のうち3方向以上が通行可能なマスか）
bool Game::IsWideCorridorTile(int tile_x, int tile_y) const
{
    if (!map_.IsWalkableTile(tile_x, tile_y)) {
        return false;
    }

    int walkable_count = 0;

    if (map_.IsWalkableTile(tile_x + 1, tile_y)) ++walkable_count;
    if (map_.IsWalkableTile(tile_x - 1, tile_y)) ++walkable_count;
    if (map_.IsWalkableTile(tile_x, tile_y + 1)) ++walkable_count;
    if (map_.IsWalkableTile(tile_x, tile_y - 1)) ++walkable_count;

    return walkable_count >= 3;
}

// ランダムな空きマスを見つける
TilePos Game::FindRandomFreeTile() const
{
    std::vector<TilePos> candidates;

    for (int y = 1; y < Map::H - 1; ++y) {
        for (int x = 1; x < Map::W - 1; ++x) {
            if (!CanPlaceObjectTile(x, y)) {
                continue;
            }

            candidates.push_back({ x, y });
        }
    }

    if (candidates.empty()) {
        return { -1, -1 };
    }

    return candidates[GetRand(static_cast<int>(candidates.size()) - 1)];
}

// ランダムな広い通路のマスを見つける
TilePos Game::FindRandomWideTile(
    int min_distance_from_first,
    const TilePos* first_tile) const
{
    std::vector<TilePos> candidates;

    for (int y = 1; y < Map::H - 1; ++y) {
        for (int x = 1; x < Map::W - 1; ++x) {
            if (!CanPlaceObjectTile(x, y)) {
                continue;
            }

            if (!IsWideCorridorTile(x, y)) {
                continue;
            }

            if (first_tile != nullptr) {
                const int dist = GetManhattanDistance(
                    first_tile->x,
                    first_tile->y,
                    x,
                    y);

                if (dist < min_distance_from_first) {
                    continue;
                }
            }

            candidates.push_back({ x, y });
        }
    }

    if (candidates.empty()) {
        return { -1, -1 };
    }

    return candidates[GetRand(static_cast<int>(candidates.size()) - 1)];
}

// 指定マスから一定距離内のリング再配置先を見つける
TilePos Game::FindRingRelocationTile(
    int from_tile_x,
    int from_tile_y,
    int min_distance,
    int max_distance) const
{
    int dist[Map::H][Map::W];

    DistanceMap::BuildDistanceMapTile(from_tile_x, from_tile_y, map_, dist);

    std::vector<TilePos> candidates;

    for (int y = 1; y < Map::H - 1; ++y) {
        for (int x = 1; x < Map::W - 1; ++x) {

            if (!CanPlaceObjectTile(x, y)) {
                continue;
            }

            int d = dist[y][x];

            if (d == GameConst::kDistUnreachable) {
                continue;
            }

            if (d < min_distance || d > max_distance) {
                continue;
            }

            candidates.push_back({ x, y });
        }
    }

    if (candidates.empty()) {
        return FindRandomFreeTile();
    }

    return candidates[GetRand(static_cast<int>(candidates.size()) - 1)];
}

// プレイヤーから一定距離内のリング再配置先を見つけて移動する
void Game::RelocateRingInRangeFromTile(
    int from_tile_x,
    int from_tile_y,
    int min_distance,
    int max_distance)
{
    const TilePos next = FindRingRelocationTile(
        from_tile_x,
        from_tile_y,
        min_distance,
        max_distance);

    if (next.x < 0 || next.y < 0) {
        return;
    }

    ring_.SetTilePos(next.x, next.y);
    ring_.ClearNextPos();
}

// プレイヤーから一定距離内のリング再配置先を見つけて移動する
void Game::RelocateRingFromPlayerInRange(int min_distance, int max_distance)
{
    RelocateRingInRangeFromTile(
        player_.GetTilePosX(),
        player_.GetTilePosY(),
        min_distance,
        max_distance);
}

// リングを追跡する敵を1体追加する
void Game::AddChaseEnemies()
{
    const TilePos pos = FindRandomFreeTile();

    if (pos.x < 0 || pos.y < 0) {
        return;
    }

    chasers_.emplace_back(pos.x, pos.y);
}

// 指定アイテムを1つ追加する
void Game::AddItem(ItemType type)
{
    const TilePos pos = FindRandomFreeTile();

    if (pos.x < 0 || pos.y < 0) {
        return;
    }

    items_.emplace_back(pos.x, pos.y, type);
}

// ポータルを1組（2つ）追加する
void Game::AddPortals()
{
    const TilePos first = FindRandomWideTile(0, nullptr);
    if (first.x < 0 || first.y < 0) {
        return;
    }

    portals_.emplace_back(first.x, first.y, 1);

    const TilePos second = FindRandomWideTile(6, &first);
    if (second.x < 0 || second.y < 0) {
        portals_.pop_back();
        return;
    }

    portals_.emplace_back(second.x, second.y, 0);
}

// プレイヤーがポータルに入ったかを判定し、入っていたらワープ処理を開始する
void Game::CheckPortal()
{
    if (is_warping_) {
        return;
    }

    bool hit_now = false;

    for (int i = 0; i < static_cast<int>(portals_.size()); ++i)
    {
        if (portals_[i].CheckHit(player_)) {
            hit_now = true;

            if (!player_on_portal_) {
                is_warping_ = true;
                warp_from_index_ = i;
                warp_start_time_ = GetNowHiPerformanceCount();
                SoundManager::PlaySE(SE_PORTAL);
                signal_manager_.Emit(TutorialSignal::kPortalUsed);
            }
            break;
        }
    }

    player_on_portal_ = hit_now;
}

// プレイヤーがアイテムを取得したかを判定し、取得していたら効果を適用する
void Game::CheckItem()
{
    for (int i = 0; i < static_cast<int>(items_.size()); ++i) {
        if (!items_[i].CheckHit(player_)) {
            continue;
        }

        if (items_[i].GetType() == ItemType::SLOW_ENEMY) {
            next_enemy_move_delay_++;
            delay_item_timer_ = GameConst::kItemEffectDurationFrames;
            SoundManager::PlaySE(SE_ITEM_ACCEL);
        }
        else if (items_[i].GetType() == ItemType::BOOST_ALPHABET) {
            alphabet_boost_count_++;
            SoundManager::PlaySE(SE_ITEM_SKIP);
        }

        signal_manager_.Emit(TutorialSignal::kItemPicked);
        items_.erase(items_.begin() + i);
        break;
    }
}

// 指定した種類のアイテムを持っているか
bool Game::HasItem(ItemType type) const
{
    for (const auto& item : items_) {
        if (item.GetType() == type) {
            return true;
        }
    }
    return false;
}

// 指定した種類のアイテムをいくつ持っているか
int Game::CountItems(ItemType type) const
{
    int count = 0;

    for (const auto& item : items_) {
        if (item.GetType() == type) {
            ++count;
        }
    }
    return count;
}

// アルファベットを進める（プレイヤーの表示文字を1段階進める）
void Game::UpdateAlphabet()
{
    prev_alphabet_ = current_alphabet_;

    int step = GameConst::kAlphabetNormalStep;

    if (alphabet_boost_count_ > 0) {
        step = GameConst::kAlphabetBoostStep;
        alphabet_boost_count_--;
        SoundManager::PlaySE(SE_SKIP_WORD);
    }

    for (int i = 0; i < step; ++i) {
        if (current_alphabet_ < GameConst::kAlphabetGoal) {
            player_.LevelUp();
            current_alphabet_++;
        }
    }
}

// アルファベットを戻す（プレイヤーの表示文字を1段階戻す）
void Game::DownAlphabet()
{
    prev_alphabet_ = current_alphabet_;

    if (current_alphabet_ > GameConst::kAlphabetStart) {
        player_.LevelDown();
        current_alphabet_--;
        SoundManager::PlaySE(SE_BACK_WORD);
    }
}

// アルファベットの変化に応じて敵やアイテムを出現させる
void Game::CheckSpawnPatrolEnemy()
{
    if (scene_ != SceneType::PLAY) {
        return;
    }

    if (current_alphabet_ == prev_alphabet_) {
        return;
    }

    bool spawned = false;
    bool patrol_spawned = false;
    bool portal_spawned = false;
    bool chaser_spawned = false;
    bool boost_item_spawned = false;
    bool slow_item_spawned = false;

    Map map_a = BuildGeneratedStageMap(generated_stage_a_, false);
    Map map_b = BuildGeneratedStageMap(generated_stage_b_, true);

    if (prev_alphabet_ < GameConst::kSpawnPatrol1At &&
        current_alphabet_ >= GameConst::kSpawnPatrol1At) {
        if (enemies_a_.size() < GameConst::kFirstEnemyCount &&
            enemies_b_.size() < GameConst::kFirstEnemyCount) {

            enemies_a_.emplace_back();
            enemies_a_.back().EnsureAutoRoute(map_a);

            enemies_b_.emplace_back();
            enemies_b_.back().EnsureAutoRoute(map_b);
			patrol_spawned = true;
            spawned = true;
        }
    }

    if (prev_alphabet_ < GameConst::kSpawnPatrol2At &&
        current_alphabet_ >= GameConst::kSpawnPatrol2At) {
        if (enemies_a_.size() < GameConst::kSecondEnemyCount &&
            enemies_b_.size() < GameConst::kSecondEnemyCount) {

            enemies_a_.emplace_back();
            enemies_a_.back().EnsureAutoRoute(map_a);

            enemies_b_.emplace_back();
            enemies_b_.back().EnsureAutoRoute(map_b);
            patrol_spawned = true;

            AddPortals();
            portal_spawned = true;
            spawned = true;
        }
    }

    if (prev_alphabet_ < GameConst::kSpawnChaser1At &&
        current_alphabet_ >= GameConst::kSpawnChaser1At) {
        if (chasers_.size() < GameConst::kFirstEnemyCount) {
            AddChaseEnemies();
            chaser_spawned = true;
            spawned = true;
        }

        if (!HasItem(ItemType::SLOW_ENEMY) &&
            enemy_move_delay_ < GameConst::kNormalEnemyMoveDelay) {
            AddItem(ItemType::SLOW_ENEMY);
            slow_item_spawned = true;
            spawned = true;
        }
    }

    if (current_alphabet_ >= GameConst::kSpawnBoostItemAt) {
        if (!HasItem(ItemType::BOOST_ALPHABET)) {
            AddItem(ItemType::BOOST_ALPHABET);
            boost_item_spawned = true;
            spawned = true;
        }
    }

    if (prev_alphabet_ < GameConst::kSpawnChaser2At &&
        current_alphabet_ >= GameConst::kSpawnChaser2At) {
        if (chasers_.size() < GameConst::kSecondEnemyCount) {
            AddChaseEnemies();
            chaser_spawned = true;
            spawned = true;
        }

        if (enemy_move_delay_ < GameConst::kNormalEnemyMoveDelay) {
            if (CountItems(ItemType::SLOW_ENEMY) < GameConst::kSecondEnemyCount) {
                AddItem(ItemType::SLOW_ENEMY);
                slow_item_spawned = true;
                spawned = true;
            }
        }
        else if (enemy_move_delay_ == GameConst::kNormalEnemyMoveDelay + 1) {
            if (!HasItem(ItemType::SLOW_ENEMY)) {
                AddItem(ItemType::SLOW_ENEMY);
                slow_item_spawned = true;
                spawned = true;
            }
        }
    }

    if (prev_alphabet_ < GameConst::kSpawnPortalAt &&
        current_alphabet_ >= GameConst::kSpawnPortalAt) {
        AddPortals();
        portal_spawned = true;
        spawned = true;
    }

    if (prev_alphabet_ < GameConst::kSpawnChaser3At &&
        current_alphabet_ >= GameConst::kSpawnChaser3At) {
        if (chasers_.size() < GameConst::kThirdChaserCount) {
            AddChaseEnemies();
            chaser_spawned = true;
            spawned = true;
        }
    }

	// 何が出現したかをメッセージで表示する
    if (spawned) {
        SoundManager::PlaySE(SE_ENEMY_SPAWN);

        char message[128] = "";
        bool has_prev = false;

        if (portal_spawned) {
            strcat_s(message, "「ワープポータル」");
            has_prev = true;
        }

        if (patrol_spawned) {
            if (has_prev) {
                strcat_s(message, "と");
            }
            strcat_s(message, "「巡回者」");
            has_prev = true;
        }

        if (chaser_spawned) {
            if (has_prev) {
                strcat_s(message, "と");
            }
            strcat_s(message, "「追跡者」");
            has_prev = true;
        }

        if (boost_item_spawned) {
            if (has_prev) {
                strcat_s(message, "と");
            }
            strcat_s(message, "「ワード促進アイテム」");
            has_prev = true;
        }

        if (slow_item_spawned) {
            if (has_prev) {
                strcat_s(message, "と");
            }
            strcat_s(message, "「移動加速アイテム」");
            has_prev = true;
        }

        if (has_prev) {
            strcat_s(message, "が出現しました");
            ShowSpawnMessage(message);
        }
    }
}

// 裏世界と表世界を切り替える
void Game::ToggleWorld()
{
    if (!CanSwitchCurrentRingPosBetweenGeneratedStages()) {
        return;
    }

    is_alt_world_ = !is_alt_world_;
    map_.SetAltWorld(is_alt_world_);
    ApplyCurrentGeneratedMap();

    if (is_alt_world_) {
        current_enemies_ = &enemies_b_;
    }
    else {
        current_enemies_ = &enemies_a_;
    }

    portals_.clear();
    items_.clear();
    chasers_.clear();

    if (current_alphabet_ >= GameConst::kSpawnPatrol2At) {
        AddPortals();
    }

    if (current_alphabet_ >= GameConst::kSpawnChaser1At) {
        AddChaseEnemies();

        if (enemy_move_delay_ == GameConst::kGameStartEnemyMoveDelay) {
            AddItem(ItemType::SLOW_ENEMY);
        }
    }

    if (current_alphabet_ >= GameConst::kSpawnBoostItemAt) {
        AddItem(ItemType::BOOST_ALPHABET);
    }

    if (current_alphabet_ >= GameConst::kSpawnChaser2At) {
        AddChaseEnemies();

        if (enemy_move_delay_ == GameConst::kNormalEnemyMoveDelay) {
            AddItem(ItemType::SLOW_ENEMY);
            AddItem(ItemType::SLOW_ENEMY);
        }
    }

    if (current_alphabet_ >= GameConst::kSpawnPortalAt) {
        AddPortals();
    }

    if (current_alphabet_ >= GameConst::kSpawnChaser3At) {
        AddChaseEnemies();
    }

    SoundManager::PlaySE(SE_PORTAL);
    signal_manager_.Emit(TutorialSignal::kWorldShifted);
}