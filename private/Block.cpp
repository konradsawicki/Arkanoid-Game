#define _WINDOWS
#include "Block.h"
#include "Framework.h"
#include <iostream>

unsigned int Block::m_ID = 0;

Block::Block(BlockType Block_Type, int BlockSizeWidth, int BlockSizeHeight, int PosX, int PosY)
    : m_BlockDurability(Block_Type.Durability), m_PathHit(Block_Type.PathHit), m_PosX(PosX), m_PosY(PosY)
{
    m_BlockSprite = createSprite(Block_Type.PathNoHit);
    getScreenSize(m_WindowWidth, m_WindowHeight);
    m_BlockSpriteWidth = BlockSizeWidth;
    m_BlockSpriteHeight = BlockSizeHeight;
    setSpriteSize(m_BlockSprite, m_BlockSpriteWidth, m_BlockSpriteHeight);
    
    FrameSegment width1, height1;
    FrameSegment width2, height2;
    width1.start_x = m_PosX;
    width1.start_y = m_PosY;
    width1.end_x = m_PosX + m_BlockSpriteWidth;
    width1.end_y = m_PosY;
    width1.frame_id = Block::m_ID;

    height1.start_x = width1.end_x;
    height1.start_y = width1.end_y;
    height1.end_x = height1.start_x;
    height1.end_y = height1.start_y + m_BlockSpriteHeight;
    height1.frame_id = Block::m_ID;

    width2.start_x = height1.end_x;
    width2.start_y = height1.end_y;
    width2.end_x = width2.start_x - m_BlockSpriteWidth;
    width2.end_y = width2.start_y;
    width2.frame_id = Block::m_ID;

    height2.start_x = width2.end_x;
    height2.start_y = width2.end_y;
    height2.end_x = height2.start_x;
    height2.end_y = height2.start_y - m_BlockSpriteHeight;
    height2.frame_id = Block::m_ID;
    
    m_BlockFrame.width1 = width1;
    m_BlockFrame.height1 = height1;
    m_BlockFrame.width2 = width2;
    m_BlockFrame.height2 = height2;

    Block::m_ID++;
}

void Block::Draw()
{
    drawSprite(m_BlockSprite, m_PosX, m_PosY);
}

block_pair<int> Block::GetBlockSize()
{
    int width, height;
    getSpriteSize(m_BlockSprite, width, height);
    return {width, height};
}

DURABILITY Block::GetBlockDurability()
{
    return m_BlockDurability;
}

BlockFrame Block::GetBlockFrame()
{
    return m_BlockFrame;
}

unsigned int Block::GetId()
{
    return m_BlockFrame.width1.frame_id;
}

block_coords<float> Block::GetPosition()
{
    return {(float)m_PosX, (float)m_PosY};
}

void Block::SetId(unsigned int new_id)
{
    m_BlockFrame.height1.frame_id = new_id;
    m_BlockFrame.height2.frame_id = new_id;
    m_BlockFrame.width1.frame_id = new_id;
    m_BlockFrame.width2.frame_id = new_id;
}

void Block::LowerDurability()
{
    if (m_BlockDurability == DURABILITY::TWO_HIT)
    {
        m_BlockDurability = DURABILITY::ONE_HIT;
        UpdateSpriteLook();
    }
}

void Block::UpdateSpriteLook()
{
    destroySprite(m_BlockSprite);
    m_BlockSprite = createSprite(m_PathHit);
    setSpriteSize(m_BlockSprite, m_BlockSpriteWidth, m_BlockSpriteHeight);
}

Block::~Block()
{   
    if (!m_SettledAbilities.empty())
    {
        for (auto& e : m_SettledAbilities)
            (*e)->Settle(false);
    }
        
    destroySprite(m_BlockSprite);
}