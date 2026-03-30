#pragma once
#include <array>
#include "GameConstants.h"

class Map
{
public:

    // ===== 生成 =====

    Map();

    // ===== 定数 =====

    // タイルサイズとマップ全体のサイズ
    static constexpr int TILE = MapConst::kMapTile;
    static constexpr int W = MapConst::kMapWidth;
    static constexpr int H = MapConst::kMapHeight;

    // ===== 描画 =====

    // 現在のマップを描画する
    void Draw() const;

    // ===== 通行判定 =====

    // 指定したピクセル座標が通行可能かを判定する
    bool IsWalkable(int pixel_x, int pixel_y) const;

    // 指定した矩形領域が通行可能かを判定する
    bool IsWalkableRect(int pixel_x, int pixel_y, int width, int height) const;

    // 指定したタイル座標が通行可能かを判定する
    bool IsWalkableTile(int tile_x, int tile_y) const;

    // 最初に見つかった通行可能マスの位置を返す
    // 出力値はピクセル座標
    void GetFirstWalkablePos(int& out_pixel_x, int& out_pixel_y) const;

    // ===== マップデータ設定 =====

    // 外部のマップデータを現在のマップへコピーする
    void SetMapData(const int src[H][W]);

    // ===== マップ切り替え =====

    // チュートリアル用マップへ切り替える
    void UseTutorialMapA();
    void UseTutorialMapB();
    void UseTutorialMapC();
    void UseAllRoadMap();

    // ===== 裏世界状態 =====

    // 裏世界状態を設定する
    void SetAltWorld(bool is_alt_world);

    // 現在が裏世界状態かどうかを返す
    bool IsAltWorld() const { return is_alt_world_; }

private:
    // 現在のマップデータ
    int data_[H][W] = {};

    // 現在が裏世界かどうか
    bool is_alt_world_ = false;
};