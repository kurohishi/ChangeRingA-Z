#pragma once

#include "GridObject.h"
#include "Map.h"
#include "Player.h"

// アイテムの種類
enum class ItemType
{
    SLOW_ENEMY,     // 敵の動きを遅くする
    BOOST_ALPHABET  // 次のアルファベット進行を強化する
};

// マップ上に配置されるアイテム
// 位置管理、更新、描画、取得判定を担当する
class Item : public GridObject
{
public:
    // ===== 生成 =====

    // 配置するタイル座標と種類を指定して生成
    Item(int tile_x, int tile_y, ItemType type);

    // ランダムな通行可能タイルに生成
    static Item CreateRandom(const Map& map, ItemType type);

    // ===== 更新・描画 =====

    void Update();

    void Draw() const;

    // ===== 判定・取得 =====

    // プレイヤーがこのアイテムを取得したか判定
    bool CheckHit(const Player& player) const;

    // アイテムの種類を取得
    ItemType GetType() const;

private:
    // ===== アイテムの種類=====
    ItemType type_;

    // ===== 演出用パラメータ =====
    int item_frame_ = 0;
};