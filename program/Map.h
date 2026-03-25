#pragma once

class Map
{
public:
    // ===== 生成 =====

    Map();

    // ===== 定数 =====

    // タイルサイズとマップ全体のサイズ
    static constexpr int TILE = 32;
    static constexpr int W = 20;
    static constexpr int H = 15;

    // 地形ID
    static constexpr int kWall = 0;
    static constexpr int kRoad = 1;

    // ===== サイズ取得 =====

    int GetWidth() const { return W; }
    int GetHeight() const { return H; }

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
    void SetData(const int src[H][W]);

    // ===== マップ切り替え =====

    // チュートリアル用マップへ切り替える
    void UseTutorialMapA();
    void UseTutorialMapB();
    void UseTutorialMapC();

    // 通常プレイ用マップへ切り替える
    void UseMapA();
    void UseMapB();

    // ===== 裏世界状態 =====

    // 裏世界状態を設定する
    void SetAltWorld(bool is_alt_world);

    // 現在が裏世界状態かどうかを返す
    bool IsAltWorld() const { return is_alt_world_; }

    // リングがマップ切り替え可能なマス上にあるかを判定する
    bool CanSwitchRingPos(int tile_x, int tile_y) const;

private:
    // 現在のマップデータ
    int data_[H][W] = {};

    // 現在が裏世界かどうか
    bool is_alt_world_ = false;
};