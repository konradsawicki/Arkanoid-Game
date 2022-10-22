#define _WINDOWS
#include "Mouse.h"
#include "Framework.h"


Mouse::Mouse(const char* Path)
{
    m_MouseSprite = createSprite(Path);
    int ScreenWidth, ScreenHeight;
    getScreenSize(ScreenWidth, ScreenHeight);
    m_MouseSpriteWidth = ScreenWidth / 25;
    m_MouseSpriteHeight = ScreenHeight / 40;
    setSpriteSize(m_MouseSprite, m_MouseSpriteWidth, m_MouseSpriteHeight);
}

void Mouse::Draw(int x, int y)
{
    drawSprite(m_MouseSprite, x - m_MouseSpriteWidth / 2, y - m_MouseSpriteHeight / 2);
}

Mouse::~Mouse()
{
    destroySprite(m_MouseSprite);
}

