#pragma once

class Sprite;

template <typename T>
struct size
{
    T width, height;
};

template <typename T>
struct ability_pair
{
    T x, y;
};

enum class ABILITY_TYPE
{
    POSITIVE, NEGATIVE
};

class Ability
{
public:
    Ability(ABILITY_TYPE A_Type);
    void Draw();
    void SetPosition(float x, float y);
    void Drop(float ElapsedTime);
    ability_pair<float> GetPosition();
    size<unsigned int> GetSize();
    ABILITY_TYPE GetType();
    unsigned int GetId();
    void SetId(int id);
    void Settle(bool Settle);

    static unsigned int GetGlobalId();
    static void SetGlobalId(int id);


    ~Ability();
protected:
    int m_WindowWidth, m_WindowHeight;
    Sprite* m_AbilitySprite;
    unsigned int m_AbilitySpriteWidth;
    unsigned int m_AbilitySpriteHeight;
    static unsigned int m_ID;
    unsigned int m_CurrentID;

    ABILITY_TYPE m_AbilityType;
    bool m_Settle;



    float m_PosX;
    float m_PosY;
};
