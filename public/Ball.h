#pragma once

class Sprite;

template <typename T>
struct ball_pair
{
    T x, y;
};

template <typename T>
struct equation
{
    T a, b;
};

class Ball
{
public:
    Ball(const char* Path, int x, int y);
    void Draw(float ElapsedTime);
    ball_pair<float> GetBallPosition();
    void SetBallPosition(unsigned int x, unsigned int y);
    unsigned int GetBallRadius();
    ball_pair<float> GetVelocity();
    equation<float> GetPathCoefficients();

    void SetVelocity(float x, float y);
    ~Ball();

protected:
    void SetBallCenterPosition(int x, int y);
    void UpdatePathEquation();

    int m_WindowWidth, m_WindowHeight;

    Sprite* m_BallSprite;
    unsigned int m_BallSpriteWidth;
    unsigned int m_BallSpriteHeight;
    unsigned int m_BallRadius;

    float m_BallPosX;
    float m_BallPosY;
    float m_BallCenterPosX;
    float m_BallCenterPosY;

    // y = ax + b
    float a; 
    float b;

    float m_BallVelocityX;
    float m_BallVelocityY;
    float m_ElapsedTime;

};