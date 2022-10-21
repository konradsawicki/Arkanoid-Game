#pragma once

class Sprite;

class Platform
{
public:
    Platform(const char* Path);
    void Draw();
    void MoveRight(float ElapsedTime);
    void MoveLeft(float ElapsedTime);

    ~Platform();

protected:
    int m_ScreenWidth, m_ScreenHeight;

    Sprite* m_PlatformSprite;
    int m_PlatformSpriteWidth;
    int m_PlatformSpriteHeight;

    int m_PlatformPosX;
    int m_PlatformPosY;
};

