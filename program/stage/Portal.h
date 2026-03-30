#pragma once

#include "GridObject.h"
#include "Map.h"
#include "Player.h"

// ワープポータルを表すクラス
// 位置管理、描画、プレイヤーとの接触判定を担当する
class Portal : public GridObject
{
public:

    // ===== 生成 =====

    // 配置するタイル座標と接続先ペア番号を指定して生成する
    Portal(int tile_x, int tile_y, int pair_index);

    // ===== 更新・描画 =====

    void Update();
    void Draw() const;

    // ===== 判定 =====

    bool CheckHit(const Player& player) const;

    // ===== 座標設定 =====

    void SetTilePos(int tile_x, int tile_y) override;

    // ===== 情報取得 =====

    // 接続先ペア番号を返す
    int GetPairIndex() const;

private:

    // ===== 基本情報 =====

    // 接続先ポータルを識別する番号
    int pair_index_ = 0;

    // ===== 演出用パラメータ =====

    // 描画アニメーションの進行値
    float anim_frame_ = 0.0f;

    // 拡縮率に応じて波打つポータル枠を描画する
    void DrawPortalBoxWave(float scale) const;
};