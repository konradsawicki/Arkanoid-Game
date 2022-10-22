#define _WINDOWS
#include "Health.h"
#include "Framework.h"
#include <iostream>

Health::Health(const char* Path)
{
    m_HealthSprite = createSprite(Path);

    getScreenSize(m_WindowWidth, m_WindowHeight);
    m_HealthSpriteWidth = m_WindowWidth / 22;
    m_HealthSpriteHeight = m_WindowHeight / 22;
    setSpriteSize(m_HealthSprite, m_HealthSpriteWidth, m_HealthSpriteHeight);
}

void Health::Draw(int x, int y)
{
    drawSprite(m_HealthSprite, x, y);
}

health_pair<int> Health::GetHealthSize()
{
    int width, height;
    getSpriteSize(m_HealthSprite, width, height);
    return {width, height};
}

Health::~Health()
{
    std::cout << "Health destroyed\n";
    destroySprite(m_HealthSprite);
}