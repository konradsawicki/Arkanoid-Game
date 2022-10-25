#pragma once
#include "Block.h"


class Sprite;
class WallElement;

struct PlatformFrame
{
    FrameSegment width;
};

template <typename T>
struct platform_pair
{
    T x, y;
};

class Platform
{
public:
    Platform(const char* Path);
    void Draw();
    void MoveRight(float ElapsedTime, int ElementWidth, int x_Start);
    void MoveLeft(float ElapsedTime, int ElementWidth, int x_Start);
    PlatformFrame GetPlatfromFrame();
    void SetPlatformId(unsigned int id);
    platform_pair<unsigned int> GetPlatformCenter();
    ~Platform();

protected:
    int m_WindowWidth, m_WindowHeight;

    Sprite* m_PlatformSprite;
    unsigned int m_PlatformSpriteWidth;
    unsigned int m_PlatformSpriteHeight;

    unsigned int m_PlatformPosX;
    unsigned int m_PlatformPosY;

    PlatformFrame m_PlatformFrame;

};

