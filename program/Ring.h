#pragma once

#include "Map.h"

class IPosition;

// リング本体の位置管理、描画、当たり判定、再配置を担当するクラス
// 座標は基本的にピクセル座標で保持し、必要に応じてタイル座標へ変換する
class Ring {
public:
    Ring();

    // 現在位置（ピクセル座標）を返す
    int GetPixelPosX() const { return x_; }
    int GetPixelPosY() const { return y_; }

    // 現在位置をタイル座標に変換して返す
    int GetTilePosX() const { return x_ / Map::TILE; }
    int GetTilePosY() const { return y_ / Map::TILE; }

    // 現在いるタイルの中心座標を返す
    // リングは中心基準で描画・判定するため、この値をよく使う
    int GetTileCenterX() const {
        return GetTilePosX() * Map::TILE + Map::TILE / 2;
    }

    int GetTileCenterY() const {
        return GetTilePosY() * Map::TILE + Map::TILE / 2;
    }

    // 次回の移動先として予約されている位置を返す
    // 内部的にはピクセル中心座標で保持している
    int GetNextPixelPosX() const { return next_x_; }
    int GetNextPixelPosY() const { return next_y_; }

    // タイル座標を受け取り、現在位置をそのタイル中心へ設定する
    void SetTilePos(int tile_x, int tile_y);

    // タイル座標を受け取り、次回移動先をそのタイル中心として保存する
    // チュートリアルなど、移動先を固定したい場面で使う
    void SetNextTilePos(int tile_x, int tile_y);

    // アニメーション用の内部状態を更新する
    void Update();

    // リング本体と粒子エフェクトを描画する
    void Draw() const;

    // 対象がリングと同じタイル上にいるかを判定する
    // ピクセル誤差の影響を減らすため、判定はタイル単位で行う
    bool CheckHit(const IPosition& target) const;

    // リングを別の位置へ再配置する
    // 通常はプレイヤーから一定距離以上離れた通行可能マスへ移動し、
    // 次の移動先が予約されている場合はその座標を優先する
    void RelocateFar(const IPosition& target, const Map& map);

    // 次回移動先として使う予約座標（ピクセル座標、中心基準）
    int next_x_ = 0;
    int next_y_ = 0;

private:
    // リング周囲を回る粒子の数
    static constexpr int kParticleCount = 12;

    // 現在位置（ピクセル座標、中心基準）
    int x_ = 0;
    int y_ = 0;

    // リング本体の半径
    int radius_ = 0;

    // 粒子エフェクトの回転角度と回転速度
    float particle_angle_ = 0.0f;
    float particle_speed_ = 0.01f;
};