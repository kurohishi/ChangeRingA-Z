#pragma once

// 位置情報を持つオブジェクト用の共通インターフェース
class IPosition{

public:
    virtual ~IPosition() = default;

    // 現在のピクセル座標を返す
    virtual int GetPixelPosX() const = 0;
    virtual int GetPixelPosY() const = 0;

    // 現在のタイル座標を返す
    virtual int GetTilePosX() const = 0;
    virtual int GetTilePosY() const = 0;
};
