#pragma once

class Map {
public:

    // タイルとマップのサイズ
    static const int TILE = 32;
    static const int W = 20;
    static const int H = 15;

    int GetWidth() const { return W; }
    int GetHeight() const { return H; }

    // 現在のマップを描画する
    void Draw() const;

    // 指定したピクセル座標が通行可能かを判定する
    bool IsWalkable(int pixel_x, int pixel_y) const;

    // 指定した矩形領域が通行可能かを判定する
    bool IsWalkableRect(int pixel_x, int pixel_y, int width, int height) const;

    // 指定したタイル座標が通行可能かを判定する
    bool IsWalkableTile(int tile_x, int tile_y) const;

    // 最初に見つかった通行可能マスの位置を返す
    // 出力値はピクセル座標で返す
    void GetFirstWalkablePos(int& out_pixel_x, int& out_pixel_y) const;

    // 外部のマップデータを現在のマップへコピーする
    void SetData(const int src[H][W]);

    // チュートリアル用マップへ切り替える
    void UseTutorialMapA();
    void UseTutorialMapB();
    void UseTutorialMapC();

    // 通常プレイ用マップへ切り替える
    void UseMapA();
    void UseMapB();

    // 裏世界状態を設定する
    void SetAltWorld(bool is_alt_world);

    // 現在が裏世界状態かどうかを返す
    bool IsAltWorld() const { return is_alt_world_; }

    // リングがマップ切り替え可能なマス上にあるかを判定する
    bool CanSwitchRingPos(int tile_x, int tile_y) const;

private:

    // マップデータ(0：壁、1 ：通路)
    int data_[H][W] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0},
        {0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
        {0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0},
        {0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0},
        {0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    // 裏世界状態かどうか
    bool is_alt_world_ = false;
};