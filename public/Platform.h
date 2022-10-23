#pragma once


class Sprite;
class WallElement;

struct PlatformFrame
{
    int start_x, start_y;
    int end_x, end_y;
    unsigned int radius;
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

