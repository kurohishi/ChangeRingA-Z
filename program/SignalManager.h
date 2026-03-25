#pragma once

#include <string>
#include <unordered_set>

namespace TutorialSignal
{
    inline constexpr const char* kPlayerMoved = "player_moved";
    inline constexpr const char* kRingEntered = "ring_entered";
    inline constexpr const char* kPatrolEnemyHit = "enemy_hit";
    inline constexpr const char* kPortalUsed = "portal_used";
    inline constexpr const char* kWorldShifted = "world_shifted";
    inline constexpr const char* kItemPicked = "item_picked";
    inline constexpr const char* kDialogueAdvanced = "dialogue_advanced";
    inline constexpr const char* kTutorialSkipped = "tutorial_skipped";
}

// ゲーム内の進行シグナルを管理するクラス
class SignalManager
{
public:
    // シグナルを送る
    void Emit(const std::string& signal);

    // 指定したシグナルが存在するか確認する
    bool Has(const std::string& signal) const;

    // 指定したシグナルを消す
    void Clear(const std::string& signal);

    // すべてのシグナルを消す
    void ClearAll();

private:
    std::unordered_set<std::string> signals_;
};