#pragma once
#include "Ability.h"
#include <memory>
#include <vector>

class Sprite;

enum class DURABILITY
{
    ONE_HIT = 1, TWO_HIT, INF_HIT

};

template <typename T>
struct block_pair
{
    T width, height;
};

template <typename T>
struct block_coords
{
    T x, y;
};

struct BlockType
{
    DURABILITY Durability;
    const char* PathNoHit;
    const char* PathHit;
};

struct FrameSegment
{
    int start_x, start_y;
    int end_x, end_y;
    int radius = 4;
    unsigned int frame_id;
};

struct BlockFrame
{
     FrameSegment width1, height1, width2, height2;
};

class Block
{
public:
    Block(BlockType Block_Type, int BlockSizeWidth, int BlockSizeHeight, int PosX, int PosY);
    void Draw();
    block_pair<int> GetBlockSize();
    DURABILITY GetBlockDurability();
    BlockFrame GetBlockFrame();
    unsigned int GetId();
    block_coords<float> GetPosition();
    void SetId(unsigned int new_id);
    void LowerDurability();
    std::vector<std::unique_ptr<Ability>*> m_SettledAbilities;
    static unsigned int m_ID;

    ~Block();

protected:
    void UpdateSpriteLook();

    
    int m_WindowWidth, m_WindowHeight;
    Sprite* m_BlockSprite;
    const char* m_PathHit;
    unsigned int m_BlockSpriteWidth;
    unsigned int m_BlockSpriteHeight;
    int m_PosX;
    int m_PosY;

    

    DURABILITY m_BlockDurability;
    BlockFrame m_BlockFrame;
    
};