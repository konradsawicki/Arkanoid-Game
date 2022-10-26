
#define _WINDOWS
#include "Ability.h"
#include "Framework.h"
#include <iostream>

Ability::Ability(ABILITY_TYPE A_Type)
    : m_Settle(false)
{
    m_AbilityType = A_Type;
    const char* Path;
    if (m_AbilityType == ABILITY_TYPE::POSITIVE)
        Path = "data/24-Breakout-Tiles.png";
    else
        Path = "data/28-Breakout-Tiles.png";

    m_AbilitySprite = createSprite(Path);

    getScreenSize(m_WindowWidth, m_WindowHeight);
    m_AbilitySpriteWidth = m_WindowWidth / 40;
    m_AbilitySpriteHeight = m_WindowHeight / 40;
    setSpriteSize(m_AbilitySprite, m_AbilitySpriteWidth, m_AbilitySpriteHeight);

}

void Ability::Draw()
{
    drawSprite(m_AbilitySprite, m_PosX, m_PosY);
}

void Ability::SetPosition(float x, float y)
{
    m_PosX = x - (float)m_AbilitySpriteWidth / 2.0f;
    m_PosY = y - (float)m_AbilitySpriteHeight / 2.0f;
}

void Ability::Drop(float ElapsedTime)
{
    if (!m_Settle)
        m_PosY += ElapsedTime / 5.0f;
}

ability_pair<float> Ability::GetPosition()
{
    return {m_PosX, m_PosY};
}

size<unsigned int> Ability::GetSize()
{
    return {m_AbilitySpriteWidth, m_AbilitySpriteHeight};
}

Ability::~Ability()
{
    destroySprite(m_AbilitySprite);
}

ABILITY_TYPE Ability::GetType()
{
    return m_AbilityType;
}

void Ability::Settle(bool Settle)
{
    m_Settle = Settle;
}

