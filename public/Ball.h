#pragma once

class Sprite;

template <typename T>
struct ball_pair
{
    T x, y;
};

class Ball
{
public:
    Ball(const char* Path);
    void Draw(int x, int y);
    ball_pair<unsigned int> GetBallPosition();
    unsigned int GetBallRadius();
    void LowerVelocity();
    void IncreaseVelocity();
    ~Ball();

protected:
    void SetBallCenterPosition(int x, int y);

    int m_WindowWidth, m_WindowHeight;

    Sprite* m_BallSprite;
    unsigned int m_BallSpriteWidth;
    unsigned int m_BallSpriteHeight;
    unsigned int m_BallRadius;

    unsigned int m_BallPosX;
    unsigned int m_BallPosY;

    int m_BallVelocityX;
    int m_BallVelocityY;

};