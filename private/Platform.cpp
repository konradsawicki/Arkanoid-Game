#define _WINDOWS
#include "Platform.h"
#include "Framework.h"
#include <iostream>

Platform::Platform(const char* Path, int PosX)
{
    m_PlatformSprite = createSprite(Path);

    getScreenSize(m_WindowWidth, m_WindowHeight);
    m_PlatformSpriteWidth = m_WindowWidth / 8;
    m_PlatformSpriteHeight = m_WindowHeight / 19;
    setSpriteSize(m_PlatformSprite, m_PlatformSpriteWidth, m_PlatformSpriteHeight);
    m_PlatformPosX = PosX - m_PlatformSpriteWidth / 2;
    m_PlatformPosY = m_WindowHeight - m_WindowHeight / 7;

    m_PlatformFrame.width.radius = m_PlatformSpriteHeight / 2;
    m_PlatformFrame.width.start_x = m_PlatformPosX + m_PlatformFrame.width.radius;
    m_PlatformFrame.width.start_y = m_PlatformPosY + m_PlatformFrame.width.radius;
    m_PlatformFrame.width.end_x = m_PlatformPosX + m_PlatformSpriteWidth - m_PlatformFrame.width.radius;
    m_PlatformFrame.width.end_y = m_PlatformFrame.width.start_y;
}

void Platform::Draw()
{  
    drawSprite(m_PlatformSprite, m_PlatformPosX, m_PlatformPosY);
}

void Platform::MoveRight(float ElapsedTime, int ElementWidth, int x_Start)
{   
    if (m_PlatformPosX < m_WindowWidth - x_Start - ElementWidth - m_PlatformSpriteWidth)
    {
        m_PlatformPosX += (m_WindowWidth / 600) * ElapsedTime;
        m_PlatformFrame.width.start_x += (m_WindowWidth / 600) * ElapsedTime;
        m_PlatformFrame.width.end_x += (m_WindowWidth / 600) * ElapsedTime;
    }
}

void Platform::MoveLeft(float ElapsedTime, int ElementWidth, int x_Start)
{
    if (m_PlatformPosX > ElementWidth)
    {
        m_PlatformPosX -= (m_WindowWidth / 600) * ElapsedTime;
        m_PlatformFrame.width.start_x -= (m_WindowWidth / 600) * ElapsedTime;
        m_PlatformFrame.width.end_x -= (m_WindowWidth / 600) * ElapsedTime;
    }
}

PlatformFrame Platform::GetPlatfromFrame()
{
    return m_PlatformFrame;
}

void Platform::SetPlatformId(unsigned int id)
{
    m_PlatformFrame.width.frame_id = id;
}

platform_pair<unsigned int> Platform::GetPlatformCenter()
{
    return {m_PlatformPosX + m_PlatformSpriteWidth / 2, m_PlatformPosY};
}

platform_size<float> Platform::GetSize()
{
    return {(float)m_PlatformSpriteWidth, (float)m_PlatformSpriteHeight};
}

platform_pair<float> Platform::GetPosition()
{
    return {(float)m_PlatformPosX, (float)m_PlatformPosY};
}

Platform::~Platform()
{
    destroySprite(m_PlatformSprite);
}
