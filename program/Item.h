#pragma once

#include "Player.h"

// アイテムの種類
enum class ItemType {
    SLOW_ENEMY,
    BOOST_ALPHABET
};

// マップ上に配置されるアイテム ※位置管理、描画、取得判定を担当
class Item {
public:
    // 配置タイル座標と種類を指定してアイテムを生成する
    Item(int tile_x, int tile_y, ItemType type);

    // アニメーションなどの内部状態を更新する
    void Update();

    // アイテムを描画する
    void Draw() const;

    // プレイヤーがこのアイテムを取得したかどうかを判定する
    bool CheckHit(const Player& player) const;

    // アイテムの種類を返す
    ItemType GetType() const;

    // 配置位置をタイル座標で返す
    int GetTileX() const;
    int GetTileY() const;

private:

    // 配置位置（タイル座標）
    int x_ = 0;
    int y_ = 0;

    // アイテムの種類
    ItemType type_;

    // 浮遊や点滅などの演出に使うアニメーション用パラメータ
    float anim_ = 0.0f;

    // スプライト切り替えや演出段階の管理に使うフレーム値
    int item_frame_ = 0;
};