#pragma once

#include "IPosition.h"
#include "Map.h"
#include "Player.h"

// アイテムの種類
enum class ItemType {
    SLOW_ENEMY,
    BOOST_ALPHABET
};

// マップ上に配置されるアイテム ※位置管理、描画、取得判定を担当
class Item : public IPosition {

public:
    // 配置タイル座標と種類を指定してアイテムを生成する
    Item(int tile_x, int tile_y, ItemType type);

    // ===== IPosition 実装 =====

// 現在位置（ピクセル座標）を返す
    int GetPixelPosX() const override { return x_; }
    int GetPixelPosY() const override { return y_; }

    // 現在位置をタイル座標に変換して返す
    int GetTilePosX() const override { return x_ / Map::TILE; }
    int GetTilePosY() const override { return y_ / Map::TILE; }

    // アニメーションなどの内部状態を更新する
    void Update();

    // プレイヤーがこのアイテムを取得したかどうかを判定する
    bool CheckHit(const Player& player) const;

    // アイテムを描画する
    void Draw() const;

    // アイテムの種類を返す
    ItemType GetType() const;

    static Item CreateRandom(const Map& map, ItemType type);

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