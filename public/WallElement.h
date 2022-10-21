#pragma once

class Sprite;

template<typename T>
struct pair
{
    T width, height;
};

class WallElement
{
public:
    WallElement(const char* Path, unsigned int NumberOfWallElementsWidth, 
        unsigned int NumberOfWallElementsHeight);
    void Draw(int x, int y);
    pair<int> GetWallElementSize();

protected:
    int m_WindowWidth, m_WindowHeight;

    Sprite* m_WallElementSprite;
    unsigned int m_WallElementSpriteWidth;
    unsigned int m_WallElementSpriteHeight;
};