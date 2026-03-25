#pragma once

#include "Map.h"
#include "Constants.h"

class IPosition;

// リングの位置管理、描画、当たり判定、再配置を担当するクラス
// 座標は基本的にピクセル座標で保持し、必要に応じてタイル座標へ変換する
class Ring
{
public:
    // ===== 生成 =====

    Ring();

    // ===== 座標取得 =====

    // 現在のピクセル座標を返す
    int GetPixelPosX() const { return x_; }
    int GetPixelPosY() const { return y_; }

    // 現在のタイル座標を返す
    int GetTilePosX() const { return x_ / Map::TILE; }
    int GetTilePosY() const { return y_ / Map::TILE; }

    // 現在いるタイルの中心座標を返す
    int GetTileCenterX() const
    {
        return GetTilePosX() * Map::TILE + Map::TILE / 2;
    }

    int GetTileCenterY() const
    {
        return GetTilePosY() * Map::TILE + Map::TILE / 2;
    }

    // 次回移動先として予約されているピクセル座標を返す
    int GetNextPixelPosX() const { return next_x_; }
    int GetNextPixelPosY() const { return next_y_; }

    // ===== 座標設定 =====

    // 現在位置を指定タイルの中心に設定する
    void SetTilePos(int tile_x, int tile_y);

    // 次回移動先を指定タイルの中心として予約する
    void SetNextTilePos(int tile_x, int tile_y);

    // 次回移動先の予約を解除する
    void ClearNextPos();

    // ===== 更新・描画 =====

    // アニメーション用の内部状態を更新する
    void Update();

    // リング本体と粒子エフェクトを描画する
    void Draw() const;

    // ===== 判定・再配置 =====

    // 対象がリングと同じタイル上にいるかを判定する
    bool CheckHit(const IPosition& target) const;

    // リングを別の位置へ再配置する
    void RelocateFar(const IPosition& target, const Map& map);

private:
    // リング周囲を回る粒子の数
    static constexpr int kParticleCount = RingConst::kParticleCount;

    // ===== 基本情報 =====

    // 現在位置（ピクセル座標、中心基準）
    int x_ = 0;
    int y_ = 0;

    // 次回移動先として使う予約座標（ピクセル座標、中心基準）
    int next_x_ = 0;
    int next_y_ = 0;

    // リング本体の半径
    int radius_ = 0;

    // ===== 演出用パラメータ =====

    // 粒子エフェクトの回転角度と回転速度
    float particle_angle_ = 0.0f;
    float particle_speed_ = RingConst::kParticleSpeed;
};