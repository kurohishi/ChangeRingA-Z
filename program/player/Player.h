#pragma once

#include "GameConstants.h"
#include "GridObject.h"
#include "Map.h"

// プレイヤーの位置、移動、描画、状態を管理するクラス
// 座標は基本的にピクセル座標で保持し、必要に応じてタイル座標へ変換する
class Player : public GridObject
{
public:

    // ===== 生成 =====

    // 初期位置を設定し、通行可能な位置へ補正する
    explicit Player(const Map& map);

    // ===== 更新・描画 =====

    bool Update(const Map& map);
    void Draw();

    // ===== 描画位置管理 =====

    // 描画位置が前回から変わったかを判定する
    bool HasDrawPositionChanged() const;

    // 現在の描画位置を前回描画位置として保存する
    void CommitDrawPosition();

    // ===== 状態操作 =====

    // 表示文字を1段階進める
    void LevelUp();

    // 表示文字を1段階戻す
    void LevelDown();

    // 表示文字が Z かどうかを返す
    bool IsZ() const;

    // ===== 座標設定 =====

    // タイル左上基準で位置を設定する
    void SetTilePos(int tile_x, int tile_y) override;

    // タイル中心基準で位置を設定する
    void SetTileCenterPos(int tile_x, int tile_y);

    // ===== 座標取得補助 =====

    // 現在いるタイルの中心座標を返す
    int GetTileCenterX() const
    {
        return GetTilePosX() * MapConst::kMapTile + MapConst::kMapTile / 2;
    }

    int GetTileCenterY() const
    {
        return GetTilePosY() * MapConst::kMapTile + MapConst::kMapTile / 2;
    }

    // リング関連の補助座標を返す
    int GetRingX() const { return ring_x_; }
    int GetRingY() const { return ring_y_; }

private:

    // リングとの判定や移動補助に使う座標（ピクセル座標）
    int ring_x_ = 0;
    int ring_y_ = 0;

    // 現在フレームの描画中心座標
    int draw_center_x_ = 0;
    int draw_center_y_ = 0;

    // 前回フレームの描画中心座標
    int prev_draw_center_x_ = 0;
    int prev_draw_center_y_ = 0;

	// プレイヤーの移動速度
    int speed_ = 0;

    // 表示中の文字
    char character_ = PlayerConst::kNoCharacter;

    // プレイヤーの文字サイズ
    int font_size_ = 0;

    // 移動入力間隔などの制御用
    int move_wait_ = 0;
};