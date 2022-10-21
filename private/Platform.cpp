#define _WINDOWS
#include "Platform.h"
#include "Framework.h"

Platform::Platform(const char* Path)
{
    m_PlatformSprite = createSprite(Path);
    
    int ScreenWidth, ScreenHeight;
    getScreenSize(ScreenWidth, ScreenHeight);
    m_PlatformSpriteWidth = ScreenWidth / 8;
    m_PlatformSpriteHeight = ScreenHeight / 19;
    setSpriteSize(m_PlatformSprite, m_PlatformSpriteWidth, m_PlatformSpriteHeight);
}

void Platform::Draw(int x, int y)
{
    drawSprite(m_PlatformSprite, x, y);
}

Platform::~Platform()
{
    
}
