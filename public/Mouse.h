#pragma once

class Sprite;

class Mouse
{
public:
    Mouse(const char* Path);
    void Draw(int x, int y);

protected: 
    Sprite* m_MouseSprite;
    int m_MouseSpriteWidth;
    int m_MouseSpriteHeight;
};