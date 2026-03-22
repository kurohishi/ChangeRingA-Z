#pragma once

class IPosition
{
public:
    virtual int GetPixelPosX() const = 0;
    virtual int GetPixelPosY() const = 0;
    virtual int GetTilePosX() const = 0;
    virtual int GetTilePosY() const = 0;
    virtual ~IPosition() = default;
};
