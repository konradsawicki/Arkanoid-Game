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
    int m_WindowWidth, m_WindowHeight;

    Sprite* m_PlatformSprite;
    unsigned int m_PlatformSpriteWidth;
    unsigned int m_PlatformSpriteHeight;

    unsigned int m_PlatformPosX;
    unsigned int m_PlatformPosY;
};

