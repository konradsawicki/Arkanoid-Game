#define _WINDOWS
#include "Ball.h"
#include "Framework.h"

Ball::Ball(const char* Path)
      :  m_BallVelocityX(0), m_BallVelocityY(0)
{
    m_BallSprite = createSprite(Path);
    getScreenSize(m_WindowWidth, m_WindowHeight);
    m_BallSpriteWidth = m_WindowWidth / 40;
    m_BallSpriteHeight = m_WindowHeight / 40;
    setSpriteSize(m_BallSprite, m_BallSpriteWidth, m_BallSpriteHeight);
    m_BallRadius = m_BallSpriteWidth / 2 - 2;
}

void Ball::SetBallCenterPosition(int x, int y)
{
    m_BallPosX = x - m_BallSpriteWidth / 2;
    m_BallPosY = y - m_BallSpriteHeight / 2;
}

void Ball::Draw(int x, int y)
{
    SetBallCenterPosition(x, y);
    drawSprite(m_BallSprite, m_BallPosX, m_BallPosY);
}

ball_pair<unsigned int> Ball::GetBallPosition()
{
    return {m_BallPosX, m_BallPosY};
}

unsigned int Ball::GetBallRadius()
{
    return m_BallRadius;
}

Ball::~Ball()
{
    destroySprite(m_BallSprite);
}