#define _WINDOWS
#include "Ball.h"
#include "Framework.h"
#include <thread>
#include <chrono>
#include <iostream>

Ball::Ball(const char* Path, int x, int y)
      :  m_BallVelocityX(0), m_BallVelocityY(0)
{
    m_BallSprite = createSprite(Path);
    getScreenSize(m_WindowWidth, m_WindowHeight);
    m_BallSpriteWidth = m_WindowWidth / 40;
    m_BallSpriteHeight = m_WindowHeight / 40;
    setSpriteSize(m_BallSprite, m_BallSpriteWidth, m_BallSpriteHeight);
    m_BallRadius = m_BallSpriteWidth / 2 - 2;

    SetBallCenterPosition(x, y);
    m_BallPosY -= m_BallRadius;
}

void Ball::SetBallCenterPosition(int x, int y)
{
    m_BallPosX = x - m_BallSpriteWidth / 2;
    m_BallPosY = y - m_BallSpriteHeight / 2;
}

void Ball::Draw(float ElapsedTime)
{
    m_BallPosX += m_BallVelocityX * ElapsedTime;
    m_BallPosY += m_BallVelocityY * ElapsedTime;
    UpdatePathEquation();
    drawSprite(m_BallSprite, m_BallPosX, m_BallPosY);
    //std::cout << "GetPathCoefficients().a=" << GetPathCoefficients().a << std::endl;
    //std::cout << "GetPathCoefficients().b=" << GetPathCoefficients().b << std::endl;
}

void Ball::SetBallPosition(unsigned int x, unsigned int y)
{
    SetBallCenterPosition(x, y);
}

ball_pair<float> Ball::GetBallPosition()
{
    m_BallCenterPosX = m_BallPosX + m_BallSpriteWidth / 2;
    m_BallCenterPosY = m_BallPosY + m_BallSpriteHeight / 2;
    return {m_BallCenterPosX, m_BallCenterPosY};
}

unsigned int Ball::GetBallRadius()
{
    return m_BallRadius;
}

ball_pair<float> Ball::GetVelocity()
{
    return {m_BallVelocityX, m_BallVelocityY};
}

void Ball::SetVelocity(float v_x, float v_y)
{
    m_BallVelocityX = v_x;
    m_BallVelocityY = v_y;
}

equation<float> Ball::GetPathCoefficients()
{
    return {a, b};
}

void Ball::UpdatePathEquation()
{
    float x_eq = m_BallCenterPosX;
    float y_eq = m_BallCenterPosY;

    if (m_BallVelocityX != 0.0f)
        a = m_BallVelocityY / m_BallVelocityX;
    else
        a = 0;
    b = y_eq - a * x_eq;

    //std::cout << "a=" << a << std::endl;
    //std::cout << "b=" << b << std::endl;
}

Ball::~Ball()
{
    destroySprite(m_BallSprite);
}