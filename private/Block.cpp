#define _WINDOWS
#include "Block.h"
#include "Framework.h"
#include <iostream>

Block::Block(BlockType Block_Type, int BlockSizeWidth, int BlockSizeHeight, int PosX, int PosY)
    : m_BlockDurability(Block_Type.Durability), m_PathHit(Block_Type.PathHit), m_PosX(PosX), m_PosY(PosY)
{
    m_BlockSprite = createSprite(Block_Type.PathNoHit);
    getScreenSize(m_WindowWidth, m_WindowHeight);
    m_BlockSpriteWidth = BlockSizeWidth;
    m_BlockSpriteHeight = BlockSizeHeight;
    setSpriteSize(m_BlockSprite, m_BlockSpriteWidth, m_BlockSpriteHeight);
    
    BlockFrameSegment width1, height1;
    BlockFrameSegment width2, height2;
    width1.start_x = m_PosX;
    width1.start_y = m_PosY;
    width1.end_x = m_PosX + m_BlockSpriteWidth;
    width1.end_y = m_PosY;

    height1.start_x = width1.end_x;
    height1.start_y = width1.end_y;
    height1.end_x = height1.start_x;
    height1.end_y = height1.start_y + m_BlockSpriteHeight;

    width2.start_x = height1.end_x;
    width2.start_y = height1.end_y;
    width2.end_x = width2.start_x - m_BlockSpriteWidth;
    width2.end_y = width2.start_y;

    height2.start_x = width2.end_x;
    height2.start_y = width2.end_y;
    height2.end_x = height2.start_x;
    height2.end_y = height2.start_y - m_BlockSpriteHeight;
    
    m_BlockFrame.push_back(width1);
    m_BlockFrame.push_back(height1);
    m_BlockFrame.push_back(width2);
    m_BlockFrame.push_back(height2);
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
    std::cout << "Block " << m_PosX << " | " << m_PosY << std::endl;
    destroySprite(m_BlockSprite);
}