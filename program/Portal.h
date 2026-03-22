#pragma once

#include "Map.h"
#include "Player.h"

// ワープポータルを表すクラス
// 位置、描画、プレイヤーとの接触判定を担当する
class Portal : public IPosition {

public:

    // 配置位置と接続先ペア番号を指定してポータルを生成する
    Portal(int tile_x, int tile_y, int pair_index);

    // アニメーション用の内部状態を更新する
    void Update();

    // ポータルを描画する
    void Draw();

    // プレイヤーがポータルに入ったかどうかを判定する
    bool CheckHit(const Player& player) const;

    // ===== IPosition 実装 =====

    // 現在位置（ピクセル座標）を返す
    int GetPixelPosX() const override { return x_; }
    int GetPixelPosY() const override { return y_; }

    // 現在位置をタイル座標に変換して返す
    int GetTilePosX() const override { return x_ / Map::TILE; }
    int GetTilePosY() const override { return y_ / Map::TILE; }

    // 接続先ペア番号を返す
    int GetPairIndex() const;

    // 配置位置をタイル座標で設定する
    void SetTilePos(int tile_x, int tile_y);

private:
    // 配置位置（ピクセル座標）
    int x_ = 0;
    int y_ = 0;

    // 接続先ポータルを識別するための番号
    int pair_index_ = 0;

    // 描画アニメーション用の進行値
    float anim_ = 0.0f;

    // 拡縮率を受け取り、波打つようなポータル枠を描画する
    void DrawPortalBoxWave(float scale) const;
};