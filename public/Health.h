#pragma once

class Sprite;

template<typename T>
struct health_pair
{
    T width, height;
};

class Health
{
public:
    Health(const char* Path);
    void Draw(int x, int y);
    health_pair<int> GetHealthSize();

    ~Health();

protected:
    int m_WindowWidth, m_WindowHeight;
    Sprite* m_HealthSprite;
    unsigned int m_HealthSpriteWidth;
    unsigned int m_HealthSpriteHeight;
};