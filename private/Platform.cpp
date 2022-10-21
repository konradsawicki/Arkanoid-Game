#define _WINDOWS
#include "Platform.h"
#include "Framework.h"
#include <iostream>


Platform::Platform(const char* Path)
{
    m_PlatformSprite = createSprite(Path);

    getScreenSize(m_ScreenWidth, m_ScreenHeight);
    m_PlatformSpriteWidth = m_ScreenWidth / 8;
    m_PlatformSpriteHeight = m_ScreenHeight / 19;
    setSpriteSize(m_PlatformSprite, m_PlatformSpriteWidth, m_PlatformSpriteHeight);
    m_PlatformPosX = m_ScreenWidth / 2 - m_PlatformSpriteWidth / 2;
    m_PlatformPosY = m_ScreenHeight - m_ScreenHeight / 7;
}

void Platform::Draw()
{  
    drawSprite(m_PlatformSprite, m_PlatformPosX, m_PlatformPosY);
}

void Platform::MoveRight(float ElapsedTime)
{
    if (m_PlatformPosX < m_ScreenWidth - m_PlatformSpriteWidth)
        m_PlatformPosX += (m_ScreenWidth / 600) * ElapsedTime;
}

void Platform::MoveLeft(float ElapsedTime)
{
    if (m_PlatformPosX > 0)
        m_PlatformPosX -= (m_ScreenWidth / 600) * ElapsedTime;
}

Platform::~Platform()
{
    
}
