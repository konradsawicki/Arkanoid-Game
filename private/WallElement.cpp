#define _WINDOWS
#include "WallElement.h"
#include "Framework.h"

WallElement::WallElement(const char* Path, unsigned int NumberOfWallElementsWidth, 
    unsigned int NumberOfWallElementsHeight)
{
    m_WallElementSprite = createSprite(Path);

    getScreenSize(m_WindowWidth, m_WindowHeight);
    m_WallElementSpriteWidth = m_WindowWidth / NumberOfWallElementsWidth;
    m_WallElementSpriteHeight = m_WindowHeight / NumberOfWallElementsHeight;
    setSpriteSize(m_WallElementSprite, m_WallElementSpriteWidth, m_WallElementSpriteHeight);
}

void WallElement::Draw(int x, int y)
{
    drawSprite(m_WallElementSprite, x, y);
}

pair<int> WallElement::GetWallElementSize()
{
    int width, height;
    getSpriteSize(m_WallElementSprite, width, height);
    return {width, height};
}