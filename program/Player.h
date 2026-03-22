#pragma once

#include "IPosition.h"
#include "Map.h"

// プレイヤーの位置・描画・状態管理を行うクラス
// 座標は基本的にピクセル座標で保持し、必要に応じてタイル座標へ変換する
class Player : public IPosition {

public:

    // 初期位置を設定し、通行可能な位置へ補正する
    explicit Player(const Map& map);

    // 入力に応じて移動処理を行う
    // 戻り値：位置が変化した場合 true
    bool Update(const Map& map);

    // プレイヤーを文字として描画する
    void Draw();

    // 描画位置が前回から変わったかを判定する
    bool HasDrawPositionChanged() const;

    // 現在の描画位置を「前回の描画位置」として保存する
    void CommitDrawPosition();

    // 文字レベルの増減（A?Z）
    void LevelUp();
    void LevelDown();
    bool IsZ() const;

    // ===== IPosition 実装 =====

    // 現在位置（ピクセル座標）を返す
    int GetPixelPosX() const override { return x_; }
    int GetPixelPosY() const override { return y_; }

    // 現在位置をタイル座標に変換して返す
    int GetTilePosX() const override { return x_ / Map::TILE; }
    int GetTilePosY() const override { return y_ / Map::TILE; }

    // ===== サイズ情報 =====

    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }

    // ===== 座標補助 =====

    // 現在いるタイルの中心座標（描画・移動判定で使用）
    int GetTileCenterX() const {
        return GetTilePosX() * Map::TILE + Map::TILE / 2;
    }

    int GetTileCenterY() const {
        return GetTilePosY() * Map::TILE + Map::TILE / 2;
    }

    // リングとのインタラクション用に保持している座標
    // ※次に移動する位置や判定用として使われる
    int GetRingX() const { return ring_x_; }
    int GetRingY() const { return ring_y_; }

    // タイル左上基準で位置を設定
    void SetTilePos(int tile_x, int tile_y);

    // タイル中心基準で位置を設定
    void SetTileCenterPos(int tile_x, int tile_y);

private:
    // ===== 基本座標（ピクセル座標） =====

    // プレイヤーの現在位置（基準は中心）
    int x_ = 0;
    int y_ = 0;

    // ===== サイズ =====

    int width_ = 24;
    int height_ = 24;

    // ===== リング関連 =====

    // リングとの判定・移動先として使う補助座標（ピクセル）
    int ring_x_ = 0;
    int ring_y_ = 0;

    // ===== 描画関連 =====

    // 今フレームの描画位置（中心座標）
    int draw_center_x_ = 0;
    int draw_center_y_ = 0;

    // 前回描画時の位置（差分検出用）
    int prev_draw_center_x_ = 0;
    int prev_draw_center_y_ = 0;

    // ===== 状態 =====

    int speed_ = 0;

    // 表示する文字（A?Z）
    char character_ = '\0';

    // 描画サイズ
    int font_size_ = 0;

    // 移動入力の間隔制御などに使用（現在は未使用の可能性あり）
    int move_wait_ = 0;
};