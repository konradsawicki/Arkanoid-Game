#pragma once

class Sprite;

class Platform
{
public:
    Platform(const char* Path);
    void Draw(int x, int y);

    ~Platform();

protected:
    Sprite* m_PlatformSprite;
    int m_PlatformSpriteWidth;
    int m_PlatformSpriteHeight;
};

