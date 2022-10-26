#define _WINDOWS
#include "Ball.h"
#include "Framework.h"
#include <math.h>
#include <iostream>


Ball::Ball(const char* Path, int x, int y)
      :  m_BallVelocityX(0), m_BallVelocityY(0), m_ElapsedTime(1)
{
    m_BallSprite = createSprite(Path);
    getScreenSize(m_WindowWidth, m_WindowHeight);
    m_BallSpriteWidth = m_WindowWidth / 40;
    m_BallSpriteHeight = m_BallSpriteWidth;
    setSpriteSize(m_BallSprite, m_BallSpriteWidth, m_BallSpriteHeight);
    m_BallRadius = m_BallSpriteWidth / 2;

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
    m_ElapsedTime = ElapsedTime;
    m_BallPosX += m_BallVelocityX * m_ElapsedTime / 2;
    m_BallPosY += m_BallVelocityY * m_ElapsedTime / 2;
    UpdatePathEquation();
    drawSprite(m_BallSprite, m_BallPosX, m_BallPosY);
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

ball_pair<float> Ball::GetVelocity(bool withoutElapsedTime)
{
    if (withoutElapsedTime)
        return {m_BallVelocityX, m_BallVelocityY};  
    else
        return {m_BallVelocityX * m_ElapsedTime, m_BallVelocityY * m_ElapsedTime};
        
}

void Ball::SetDefaultSpeed(float speed)
{
    m_DefaultSpeed = speed;
}

void Ball::SetVelocity(float v_x, float v_y)
{
    float magnitude = sqrtf(v_x * v_x + v_y * v_y);
    if (magnitude >= 0.4f * m_DefaultSpeed && magnitude <= 3.0f * m_DefaultSpeed)
    {
        m_BallVelocityX = v_x;
        m_BallVelocityY = v_y;
    }
    else
    {
        m_BallVelocityX *= v_x / abs(v_x);
        m_BallVelocityY *= v_y / abs(v_y);
    }
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