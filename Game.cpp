
// Code written by: Konrad Sawicki
// Framework creator: Dragon's Lake
// https://github.com/konradsawicki/Arkanoid-Game

#define _WINDOWS
#define PI 3.1415
#include "Framework.h"
#include "Platform.h"
#include "Mouse.h"
#include "WallElement.h"
#include "Block.h"
#include "Health.h"
#include "Ball.h"
#include "Ability.h"
#include <regex>
#include <Windows.h>
#include <math.h>
#include <functional>
#include <algorithm>
#include <queue>
#include <string>
#include <iostream>

/* Test Framework realization */
template <typename T>
struct pair
{
	T width, height;
};

template <typename T>
struct coords
{
	T x, y;
};

struct MapFrame
{
	FrameSegment width, height1, height2;
};

class MyFramework : public Framework 
{
public:

	MyFramework(unsigned WindowWidth = 1024, unsigned WindowHeight = 768)
		: m_WindowWidth(WindowWidth), m_WindowHeight(WindowHeight),
			m_NumberOfWallElementsWidth(25), m_NumberOfWallElementsHeight(23),
				m_HealthNumber(3), m_KeyPressed(false), m_GameActive(false),
					m_PossibleCollisionWithPlatform(false), x_Start(0), y_Start(0),
						m_isVectorCalculated1(false), m_isVectorCalculated2(false),
							m_BlockDestroyedAmount(0)
	{
	}

	virtual void PreInit(int& width, int& height, bool& fullscreen) override
	{
		width = m_WindowWidth;
		height = m_WindowHeight;
		fullscreen = false;
	}

	virtual void InitPlatform()
	{
		m_Platform = std::make_unique<Platform>("data/48-Breakout-Tiles.png");
		m_Platform->SetPlatformId(m_BlockLevels.front().size() + 1);
		m_PlatformId = m_BlockLevels.front().size() + 1;
	}

	virtual void InitWallElements()
	{
		m_NumberOfWallElementsWidthReal = m_NumberOfWallElementsWidth - 6; // i don't want whole window width with Wall Elements, so i substract 6: 3 per side;
		m_NumberOfWallElementsHeightReal = m_NumberOfWallElementsHeight + 1;
		for (int i = 0; i < 2 * m_NumberOfWallElementsHeightReal + m_NumberOfWallElementsWidthReal; ++i) 
		{
			m_WallElements.push_back(std::make_unique<WallElement>("data/29-Breakout-Tiles.png", 
				m_NumberOfWallElementsWidth, m_NumberOfWallElementsHeight));
		}
		m_WallElementSize.width = m_WallElements[0]->GetWallElementSize().width;
		m_WallElementSize.height = m_WallElements[0]->GetWallElementSize().height;

		// Init map frame
		m_MapFrame.width.start_x = x_Start + m_WallElementSize.width;
		m_MapFrame.width.start_y = m_WallElementSize.height;
		m_MapFrame.width.end_x = m_MapFrame.width.start_x + m_NumberOfWallElementsWidthReal * m_WallElementSize.width;
		m_MapFrame.width.end_y = m_MapFrame.width.start_y;

		m_MapFrame.height1.start_x = m_MapFrame.width.end_x;
		m_MapFrame.height1.start_y = m_MapFrame.width.end_y;
		m_MapFrame.height1.end_x = m_MapFrame.height1.start_x;
		m_MapFrame.height1.end_y = m_MapFrame.height1.start_y + m_NumberOfWallElementsHeightReal * m_WallElementSize.height;

		m_MapFrame.height2.start_x = m_MapFrame.width.start_x;
		m_MapFrame.height2.start_y = m_MapFrame.height1.end_y;
		m_MapFrame.height2.end_x = m_MapFrame.height2.start_x;
		m_MapFrame.height2.end_y = m_MapFrame.width.start_y;

		// std::cout << "m_MapFrame.width.start_x="<< m_MapFrame.width.start_x << std::endl;
		// std::cout <<"m_MapFrame.width.start_y="<< m_MapFrame.width.start_y<< std::endl;
		// std::cout <<"m_MapFrame.width.end_x="<< m_MapFrame.width.end_x<< std::endl;
		// std::cout <<"m_MapFrame.width.end_y="<< m_MapFrame.width.end_y<< std::endl;
		// std::cout << "===================" << std::endl;
		// std::cout <<"m_MapFrame.height1.start_x="<< m_MapFrame.height1.start_x<< std::endl;
		// std::cout <<"m_MapFrame.height1.start_y="<< m_MapFrame.height1.start_y<< std::endl;
		// std::cout <<"m_MapFrame.height1.end_x="<< m_MapFrame.height1.end_x<< std::endl;
		// std::cout <<"m_MapFrame.height1.end_y="<< m_MapFrame.height1.end_y<< std::endl;
		// std::cout << "===================" << std::endl;
		// std::cout <<"m_MapFrame.height2.start_x="<< m_MapFrame.height2.start_x << std::endl;
		// std::cout <<"m_MapFrame.height2.start_y="<< m_MapFrame.height2.start_y<< std::endl;
		// std::cout <<"m_MapFrame.height2.end_x="<< m_MapFrame.height2.end_x<< std::endl;
		// std::cout <<"m_MapFrame.height2.end_y="<< m_MapFrame.height2.end_y<< std::endl;
	}

	virtual void InitHealth()
	{
		for (int i = 0; i < m_HealthNumber; ++i)
		{
			m_Health.push_back(std::make_unique<Health>("data/60-Breakout-Tiles.png"));
		}
		m_HealthSize.width = m_Health[0]->GetHealthSize().width;
		m_HealthSize.height = m_Health[0]->GetHealthSize().height;
	}

	virtual void InitBlockLevels()
	{
		unsigned int NumberOfBlocksWidth = 12;
		unsigned int NumberOfBlocksHeight = 25;
		m_BlockSize = {(int)(m_NumberOfWallElementsWidthReal * m_WallElementSize.width / NumberOfBlocksWidth), (int)(m_NumberOfWallElementsHeightReal * m_WallElementSize.height / NumberOfBlocksHeight)};

		BlockType BlockInfHit;
		BlockInfHit.Durability = DURABILITY::INF_HIT;
		BlockInfHit.PathNoHit = "data/09-Breakout-Tiles.png";
		BlockInfHit.PathHit = "data/10-Breakout-Tiles.png";

		auto RandomBlockType = [&]() 
		{
			//srand(time(nullptr)); // uncomment if you want different durability distribution each time you play
			BlockType Block_Type;
			int b_type = (rand() % 3) + 1;
			if (b_type == 1)
			{
				Block_Type.Durability = DURABILITY::ONE_HIT;
				Block_Type.PathNoHit = "data/01-Breakout-Tiles.png";
				Block_Type.PathHit = "data/02-Breakout-Tiles.png";
				return Block_Type;
			}
			else if (b_type == 2)
			{
				Block_Type.Durability = DURABILITY::TWO_HIT;
				Block_Type.PathNoHit = "data/15-Breakout-Tiles.png";
				Block_Type.PathHit = "data/16-Breakout-Tiles.png";
				return Block_Type;
			}
			else
			{
				Block_Type.Durability = DURABILITY::ONE_HIT;
				Block_Type.PathNoHit = "data/01-Breakout-Tiles.png";
				Block_Type.PathHit = "data/02-Breakout-Tiles.png";
				return Block_Type;
			}
		};
		BlockType TempBlock;
		std::vector<std::unique_ptr<Block>> Level;

		// Level 1
		int BlockOffset = (m_NumberOfWallElementsWidthReal * m_WallElementSize.width - NumberOfBlocksWidth * m_BlockSize.width) / 2;
		for (int y = m_WindowHeight / 6; y < m_WindowHeight / 4; y += m_BlockSize.height)
		{	
			int counter = 0;
			for (int x = m_WallElementSize.width + BlockOffset; x < m_NumberOfWallElementsWidthReal * m_WallElementSize.width; x += m_BlockSize.width)
			{
				counter++;

				if (counter % 5 == 0)
					TempBlock = BlockInfHit;
				else
					TempBlock = RandomBlockType();
				Level.push_back(std::make_unique<Block>(TempBlock, m_BlockSize.width, 
						m_BlockSize.height, x, y));
			}
		}
		m_BlockLevels.push(std::move(Level));

		// std::cout << "width1_st_x=" << m_BlockLevels.front()[48]->GetBlockFrame().width1.start_x << std::endl;
		// std::cout << "width1_st_y=" << m_BlockLevels.front()[48]->GetBlockFrame().width1.start_y << std::endl;
		// std::cout << "width1_end_x=" << m_BlockLevels.front()[48]->GetBlockFrame().width1.end_x << std::endl;
		// std::cout << "width1_end_y=" << m_BlockLevels.front()[48]->GetBlockFrame().width1.end_y << std::endl;
		// std::cout << "===========================\n";
		// std::cout << "height1_st_x=" << m_BlockLevels.front()[48]->GetBlockFrame().height1.start_x << std::endl;
		// std::cout << "height1_st_y=" << m_BlockLevels.front()[48]->GetBlockFrame().height1.start_y << std::endl;
		// std::cout << "height1_end_x=" << m_BlockLevels.front()[48]->GetBlockFrame().height1.end_x << std::endl;
		// std::cout << "height1_end_y=" << m_BlockLevels.front()[48]->GetBlockFrame().height1.end_y << std::endl;
		// std::cout << "===========================\n";
		// std::cout << "width2_st_x=" << m_BlockLevels.front()[48]->GetBlockFrame().width2.start_x << std::endl;
		// std::cout << "width2_st_y=" << m_BlockLevels.front()[48]->GetBlockFrame().width2.start_y << std::endl;
		// std::cout << "width2_end_x=" << m_BlockLevels.front()[48]->GetBlockFrame().width2.end_x << std::endl;
		// std::cout << "width2_end_y=" << m_BlockLevels.front()[48]->GetBlockFrame().width2.end_y << std::endl;
		// std::cout << "===========================\n";
		// std::cout << "height2_st_x=" << m_BlockLevels.front()[48]->GetBlockFrame().height2.start_x << std::endl;
		// std::cout << "height2_st_y=" << m_BlockLevels.front()[48]->GetBlockFrame().height2.start_y << std::endl;
		// std::cout << "height2_end_x=" << m_BlockLevels.front()[48]->GetBlockFrame().height2.end_x << std::endl;
		// std::cout << "height2_end_y=" << m_BlockLevels.front()[48]->GetBlockFrame().height2.end_y << std::endl;
		Level.clear();

		Block::m_ID = 0;
		// Level 2
		for (int counter = 0, y = m_WallElementSize.height; y < m_WindowHeight / 2; y += m_BlockSize.height, ++counter)
		{
			int counter2 = 0;
			for (int x = m_WallElementSize.width; x < m_NumberOfWallElementsWidthReal * m_WallElementSize.width; x += m_BlockSize.width)
			{
				if (y + m_BlockSize.height > m_WindowHeight / 2 && counter2 % 4 == 0)
					TempBlock = BlockInfHit;
				else
					TempBlock = RandomBlockType();
				Level.push_back(std::make_unique<Block>(TempBlock, m_BlockSize.width, 
						m_BlockSize.height, x, y));
				counter2++;
				if (counter2 > counter)
					break;
			}
		}
		m_BlockLevels.push(std::move(Level));
		Level.clear();

		// Level 3
		// ...
		m_MapFrame.width.frame_id = m_BlockLevels.front().size();
		m_MapFrame.height1.frame_id = m_BlockLevels.front().size();
		m_MapFrame.height2.frame_id = m_BlockLevels.front().size();
		m_MapId = m_BlockLevels.front().size();
	}

	virtual void InitMouse()
	{
		m_Mouse = std::make_unique<Mouse>("data/59-Breakout-Tiles.png");
		showCursor(true);
	}

	virtual void InitBall()
	{
		m_Ball = std::make_unique<Ball>("data/62-Breakout-Tiles.png", m_Platform->GetPlatformCenter().x, m_Platform->GetPlatformCenter().y);
	}

	virtual void InitBackground()
	{
		m_BackgroundSprite = createSprite("data/Background_space.png");
		setSpriteSize(m_BackgroundSprite, m_WindowWidth, m_WindowHeight);
	}

	virtual void InitTimePoints()
	{
		m_StartTimePoint = getTickCount();
		m_EndTimePoint = getTickCount();
	}

	virtual bool Init() override
	{	
		InitTimePoints();
		InitBackground();
		InitWallElements();
		InitBlockLevels();
		InitPlatform();
		InitHealth();
		InitMouse();
		InitBall();

		return true;
	}

	virtual void Close() override
	{

	}

	virtual void DrawBackground()
	{
		drawSprite(m_BackgroundSprite, 0, 0);
	}

	virtual void DrawWallElements()
	{
		int counter = 0;
											// +2 because of plus 2 elements from height from both sides
		x_WallOffset = (m_WindowWidth - (m_NumberOfWallElementsWidthReal + 2) * m_WallElementSize.width);
		int y1 = y_Start;
		int y2 = y_Start;
		int x = x_Start; // x = x_WallOffset;
		for (auto& e : m_WallElements)
		{	
			if (counter < m_NumberOfWallElementsHeightReal)
			{
				e->Draw(x_Start, y1);
				y1 += m_WallElementSize.height;
				counter++;
			}
			else if (counter >= m_NumberOfWallElementsHeightReal && counter < m_NumberOfWallElementsHeightReal + m_NumberOfWallElementsWidthReal)
			{
				e->Draw(x + m_WallElementSize.width, y_Start);
				x += m_WallElementSize.width;
				counter++;
			}
			else if (counter >= m_NumberOfWallElementsHeightReal + m_NumberOfWallElementsWidthReal)
			{
				e->Draw(m_WindowWidth - x_WallOffset - m_WallElementSize.width, y2);
				y2 += m_WallElementSize.height;
				counter++;
			}
		}
	}

	virtual void DrawBlockLevel()
	{
		if (!m_BlockLevels.empty())
		{		
			for (auto& e : m_BlockLevels.front())
			{
				e->Draw();
			}
		}
	}

	virtual void DrawHealth()
	{
		if (!m_Health.empty())
		{
			int HealthNumberInRow = 3;
			int x1 = (x_WallOffset - HealthNumberInRow * m_HealthSize.width) / 2;
			int x2 = (x_WallOffset - HealthNumberInRow * m_HealthSize.width) / 2;
			int counter = 0;
			for (auto& e : m_Health)
			{
				if (counter < HealthNumberInRow)
				{
					e->Draw(m_WindowWidth - x_WallOffset + x1, m_WindowHeight / 4);
					x1 += m_WallElementSize.width;
					counter++;
				}
				else
				{
					e->Draw(m_WindowWidth - x_WallOffset + x2, m_WindowHeight / 4 + m_WallElementSize.height + 10);
					x2 += m_WallElementSize.width;
					counter++;
				}
			}
		}
	}

	virtual void DrawPlatform()
	{	
		m_Platform->Draw();
	}

	virtual void DrawBall()
	{
		m_Ball->Draw(m_ElapsedTime);
	}

	virtual void DrawMouse()
	{
		m_Mouse->Draw(m_MousePosition.x, m_MousePosition.y);
	}

	virtual void DrawAbilities()
	{
		if (!m_Abilities.empty())
		{
			for (auto& e : m_Abilities)
			{
				e->Draw();
			}
		}
	}

	virtual bool Tick() override
	{	
		UpdateElapsedTime();
		UpdatePlatformPosition();
		UpdateBallPosition();
		UpdateAbilitiesPosition();

		DrawBackground();
		DrawBlockLevel();
		DrawWallElements();
		DrawHealth();
		DrawPlatform();
		DrawAbilities();
		DrawBall();
		DrawMouse();


		CheckCollision();

		return false;
	}

	virtual void UpdateElapsedTime()
	{
		m_EndTimePoint = getTickCount();
		m_ElapsedTime = float(m_EndTimePoint - m_StartTimePoint);
		m_StartTimePoint = m_EndTimePoint;
		m_SimElapsedTime = m_ElapsedTime / (float)m_SimulationUpdates;
		
	}

	virtual void UpdateBallPosition()
	{
		if (!m_GameActive)
			m_Ball->SetBallPosition(m_Platform->GetPlatformCenter().x, m_Platform->GetPlatformCenter().y - m_Ball->GetBallRadius());
		else if (m_Calculate)
		{
			DynamicResponseStatic(m_ClosestStaticFrameSegments.first, m_StaticVectorX1, m_StaticVectorY1, 
				m_TangentPosX1, m_TangentPosY1, m_isVectorCalculated1, m_isVectorCalculated2);

			if (m_ClosestStaticFrameSegments.first.frame_id != m_ClosestStaticFrameSegments.second.frame_id)
			{
				DynamicResponseStatic(m_ClosestStaticFrameSegments.second, m_StaticVectorX2, m_StaticVectorY2, 
					m_TangentPosX2, m_TangentPosY2, m_isVectorCalculated2, m_isVectorCalculated1);
			}
		}

		if (m_GameActive)
		{
			DynamicResponsePlatform(m_Platform->GetPlatfromFrame().width);
		}
			
	}

	virtual void UpdatePlatformPosition()
	{
		if (m_KeyPressed)
		{
			switch (m_CurrentKey)
			{
				case FRKey::LEFT:
					m_Platform->MoveLeft(m_ElapsedTime, x_Start + m_WallElements[0]->GetWallElementSize().width, x_WallOffset);
					break;
				case FRKey::RIGHT:
					m_Platform->MoveRight(m_ElapsedTime, x_Start + m_WallElements[0]->GetWallElementSize().width, x_WallOffset);
					// if (m_BlockLevels.size() > 1)
					// 	m_BlockLevels.pop();
					break;
				default:
					break;
			}
		}
	}

	virtual void UpdateAbilitiesPosition()
	{
		if (!m_Abilities.empty())
		{
			for (auto& ability : m_Abilities)
			{
				float ab_lower_y = ability->GetPosition().y + ability->GetSize().height;
				auto it = std::find_if(m_BlockLevels.front().begin(), m_BlockLevels.front().end(), [&](auto& block) {
					return (block->GetPosition().y - ab_lower_y <= 1.0f &&  block->GetPosition().y - ab_lower_y >= 0.0f &&
					 		(ability->GetPosition().x + ability->GetSize().width <= block->GetPosition().x + m_BlockSize.width)  &&
					 			(ability->GetPosition().x >= block->GetPosition().x));
				});

				if (it == m_BlockLevels.front().end())
					ability->Drop(m_ElapsedTime);
				else
				{
					(*it)->m_SettledAbility = &ability;
					ability->Settle(true);
					ability->SetPosition(ability->GetPosition().x + (float)ability->GetSize().width / 2.0f, (*it)->GetPosition().y - ability->GetSize().height / 2);
				}
			}
		} 
	}

	virtual float Distance(float x1, float y1, float x2, float y2)
	{
		return sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
	}

	virtual coords<float> FindStaticIntersectionPoints(FrameSegment F_Segment, int i)
	{
		float radius = m_Ball->GetBallRadius();
		float a = m_Ball->GetPathCoefficients().a;
		float b = m_Ball->GetPathCoefficients().b;

		float ball_x = m_Ball->GetBallPosition().x;
		float ball_y = m_Ball->GetBallPosition().y;

		float ball_vel_x = m_Ball->GetVelocity().x;
		float ball_vel_y = m_Ball->GetVelocity().y;

		float offset_x = radius * cosf(PI / 2 + atanf(a)) * (float)i;
		float offset_y = radius * sinf(PI / 2 + atanf(a)) * (float)i;

		float b_p = (ball_y + offset_y) - a * (ball_x + offset_x); // b coeff for parallel linear function


		if (F_Segment.start_y == F_Segment.end_y)
		{
			float p_y = (float)F_Segment.start_y;
			float p_x = (p_y - b_p) / a;
			if (Distance(p_x, p_y, ball_x + offset_x, ball_y + offset_y) > Distance(p_x, p_y,  ball_x + offset_x + ball_vel_x, ball_y + offset_y + ball_vel_y))
			{			
				if (p_x >= (float)F_Segment.start_x && p_x <= (float)F_Segment.end_x)
				{
					//std::cout << "id=" << F_Segment.frame_id << std::endl;
					//std::cout << "p_x=" << p_x << " p_y=" << p_y << std::endl;
					m_IntersectionPoints.push_back({F_Segment, {p_x, p_y}});
					
				}
				else if (p_x >= (float)F_Segment.end_x && p_x <= (float)F_Segment.start_x)
				{	
					//std::cout << "id=" << F_Segment.frame_id << std::endl;
					//std::cout << "p_x=" << p_x << " p_y=" << p_y << std::endl;
					m_IntersectionPoints.push_back({F_Segment, {p_x, p_y}});
				}
			}
		}
		else if (F_Segment.start_x == F_Segment.end_x)
		{
			float p_x = (float)F_Segment.start_x;
			float p_y = a * p_x + b_p;
			if (Distance(p_x, p_y, ball_x + offset_x, ball_y + offset_y) > Distance(p_x, p_y,  ball_x + offset_x + ball_vel_x, ball_y + offset_y + ball_vel_y))
			{
				if (p_y >= (float)F_Segment.start_y && p_y <= (float)F_Segment.end_y)
				{
					//std::cout << "id=" << F_Segment.frame_id << std::endl;
					//std::cout << "p_x=" << p_x << " p_y=" << p_y << std::endl;
					m_IntersectionPoints.push_back({F_Segment, {p_x, p_y}});
				}
				else if (p_y >= (float)F_Segment.end_y && p_y <= (float)F_Segment.start_y)
				{
					//std::cout << "id=" << F_Segment.frame_id << std::endl;
					//std::cout << "p_x=" << p_x << " p_y=" << p_y << std::endl;
					m_IntersectionPoints.push_back({F_Segment, {p_x, p_y}});
				}
			}
		}
		return {0, 0};
	}

	virtual void FindClosestPointForStaticInstance()
	{
		if (!m_IntersectionPoints.empty())
		{	
			if (m_IntersectionPoints.size() > 1)
			{
				std::sort(m_IntersectionPoints.begin(), m_IntersectionPoints.end(), 
				[&](const auto& kv1, const auto& kv2)
				{
					return Distance(kv1.second.x, kv1.second.y, m_Ball->GetBallPosition().x, m_Ball->GetBallPosition().y) < Distance(kv2.second.x, kv2.second.y, m_Ball->GetBallPosition().x, m_Ball->GetBallPosition().y);
				});
			}
			m_ClosestFrameSegmentsForStaticInstance.push_back(m_IntersectionPoints.front());

			m_IntersectionPoints.clear();
		}
	}

	virtual void FindClosestStaticFrameSegments()
	{

		if (!m_ClosestFrameSegmentsForStaticInstance.empty())
		{	
			if (m_ClosestFrameSegmentsForStaticInstance.size() > 2)
			{
				std::sort(m_ClosestFrameSegmentsForStaticInstance.begin(), m_ClosestFrameSegmentsForStaticInstance.end(), 
				[&](const auto& kv1, const auto& kv2)
				{
					return Distance(kv1.second.x, kv1.second.y, m_Ball->GetBallPosition().x, m_Ball->GetBallPosition().y) < Distance(kv2.second.x, kv2.second.y, m_Ball->GetBallPosition().x, m_Ball->GetBallPosition().y);
				});
			}
			m_ClosestStaticFrameSegments.first = m_ClosestFrameSegmentsForStaticInstance[0].first;
			m_ClosestStaticFrameSegments.second = m_ClosestFrameSegmentsForStaticInstance[1].first;
			m_ClosestFrameSegmentsForStaticInstance.clear();
			m_Calculate = true;
			// std::cout << "-----------\n";
			// std::cout << "id1=" << m_ClosestStaticFrameSegments.first.frame_id <<  std::endl;
			// std::cout << "id1_x=" << m_ClosestFrameSegmentsForStaticInstance[0].second.x << " ";
			// std::cout << "id1_y=" << m_ClosestFrameSegmentsForStaticInstance[0].second.y << std::endl;
			// std::cout << "-----------\n";
			// std::cout << "id2=" <<m_ClosestStaticFrameSegments.second.frame_id << std::endl;
			// std::cout << "id2_x=" << m_ClosestFrameSegmentsForStaticInstance[1].second.x << " ";
			// std::cout << "id2_y=" << m_ClosestFrameSegmentsForStaticInstance[1].second.y << std::endl;
			// std::cout << "-----------\n";
		}
		else
			m_Calculate = false;
	}

	virtual void CheckCollision()
	{
		if (m_Bounced)
		{
			//Collision with blocks
			for (auto& e : m_BlockLevels.front())
			{
				FindStaticIntersectionPoints(e->GetBlockFrame().width1, 1);
				FindStaticIntersectionPoints(e->GetBlockFrame().height1, 1);
				FindStaticIntersectionPoints(e->GetBlockFrame().width2, 1);
				FindStaticIntersectionPoints(e->GetBlockFrame().height2, 1);
			}
			FindClosestPointForStaticInstance();
			std::cout << "------------\n";
			for (auto& e : m_BlockLevels.front())
			{
				FindStaticIntersectionPoints(e->GetBlockFrame().width1, -1);
				FindStaticIntersectionPoints(e->GetBlockFrame().height1, -1);
				FindStaticIntersectionPoints(e->GetBlockFrame().width2, -1);
				FindStaticIntersectionPoints(e->GetBlockFrame().height2, -1);
			}
			FindClosestPointForStaticInstance();

			// Collision with walls
			FindStaticIntersectionPoints(m_MapFrame.width, 1);
			FindStaticIntersectionPoints(m_MapFrame.height1, 1);
			FindStaticIntersectionPoints(m_MapFrame.height2, 1);
			FindClosestPointForStaticInstance();

			FindStaticIntersectionPoints(m_MapFrame.width, -1);
			FindStaticIntersectionPoints(m_MapFrame.height1, -1);
			FindStaticIntersectionPoints(m_MapFrame.height2, -1);
			FindClosestPointForStaticInstance();

			// Find overall two closest static frames
			FindClosestStaticFrameSegments();
			m_Bounced = false;
		}
	}

	virtual void ShootBall()
	{
		float v_x = (float)m_MousePosition.x - m_Ball->GetBallPosition().x;
		float v_y = (float)m_MousePosition.y - m_Ball->GetBallPosition().y;
		v_x /= Distance(m_MousePosition.x, m_MousePosition.y,
									m_Ball->GetBallPosition().x, m_Ball->GetBallPosition().y);

		v_y /= Distance(m_MousePosition.x, m_MousePosition.y,
									m_Ball->GetBallPosition().x, m_Ball->GetBallPosition().y);

		float magnitude = sqrtf(v_x * v_x + v_y * v_y);
		m_Ball->SetDefaultSpeed(magnitude);
		m_Ball->SetVelocity(v_x, v_y);

	}

	virtual coords<float> TangentPosition(FrameSegment F_Seg)
	{
		float a = m_Ball->GetPathCoefficients().a;
		float b = m_Ball->GetPathCoefficients().b;
		float radius = m_Ball->GetBallRadius();
		float tanpos_x, tanpos_y;
		
		if (F_Seg.start_y == F_Seg.end_y)
		{
			if ((float)F_Seg.start_y < m_Ball->GetBallPosition().y)
				tanpos_y = F_Seg.start_y + radius;
			else if ((float)F_Seg.start_y > m_Ball->GetBallPosition().y)
				tanpos_y = F_Seg.start_y - radius;
			else
				tanpos_y = F_Seg.start_y;

			tanpos_x = (tanpos_y - b) / a;
		}
		else if (F_Seg.start_x == F_Seg.end_x)
		{
			if ((float)F_Seg.start_x < m_Ball->GetBallPosition().x)
				tanpos_x = F_Seg.start_x + radius;
			else if ((float)F_Seg.start_x > m_Ball->GetBallPosition().x)
				tanpos_x = F_Seg.start_x - radius;
			else
				tanpos_x = F_Seg.start_x;
			tanpos_y = a * tanpos_x + b;
		}
		return {tanpos_x, tanpos_y};
	}

	virtual void DynamicResponsePlatform(FrameSegment F_Seg)
	{
		float LineX1 = (float)(F_Seg.end_x - F_Seg.start_x);
		float LineY1 = float(F_Seg.end_y - F_Seg.start_y);

		float LineX2 = m_Ball->GetBallPosition().x - (float)F_Seg.start_x;
		float LineY2 = m_Ball->GetBallPosition().y - (float)F_Seg.start_y;

		float FrameSegmentLength = LineX1 * LineX1 + LineY1 * LineY1;

		float t = std::max(0.0f, std::min(FrameSegmentLength, (LineX1 * LineX2 + LineY1 * LineY2))) / FrameSegmentLength;

		float ClosestPointX = F_Seg.start_x + t * LineX1;
		float ClosestPointY = F_Seg.start_y + t * LineY1;

		float distance = Distance(ClosestPointX, ClosestPointY, m_Ball->GetBallPosition().x, m_Ball->GetBallPosition().y);

		if (distance < m_Ball->GetBallRadius() + F_Seg.radius)
		{
			// Normal unit vector
			float nx = (ClosestPointX - m_Ball->GetBallPosition().x) / distance;
			float ny = (ClosestPointY - m_Ball->GetBallPosition().y) / distance;

			// Tangent unit vector   nx*tx + ny*ty = 0 <- dot product = 0 -> perpendicular
			float tx = -ny;
			float ty = nx; 

			float dpTan1 = m_Ball->GetVelocity(true).x * tx + m_Ball->GetVelocity(true).y * ty;
			float dpTan2 = -dpTan1;

			float dpNorm1 = m_Ball->GetVelocity(true).x * nx + m_Ball->GetVelocity(true).y * ny;
			float dpNorm2 = -dpNorm1;

			float mass1 = 1.0f;
			float mass2 = 1.0f;
			float m1 = (dpNorm1 * (mass1 - mass2) + 2.0f * mass2 * dpNorm2) / (mass1 + mass2);
			float m2 = (dpNorm1 * (mass2 - mass1) + 2.0f * mass1 * dpNorm1) / (mass1 + mass2);

			float overlap = distance - m_Ball->GetBallRadius() - F_Seg.radius;
			m_Ball->SetBallPosition(m_Ball->GetBallPosition().x + overlap * nx, m_Ball->GetBallPosition().y + overlap * ny);
			if (F_Seg.frame_id == m_PlatformId)
				m_Ball->SetVelocity(tx * dpTan1 + nx * m1, ty * dpTan1 + ny * m1);
			else
			{	
				m_Ball->SetVelocity(tx * dpTan1 + nx * m1 * 1.1f, ty * dpTan1 + ny * m1 * 1.1f);
				if (F_Seg.frame_id < m_BlockLevels.front().size())
					HitBlock(F_Seg.frame_id);
			}
			m_Bounced = true;
		}
	}

	virtual void DynamicResponseStatic(FrameSegment F_Seg, float& VectorX, float& VectorY, float& TangentPosX, float& TangentPosY, bool& isVectorCalculated, bool& isVectorCalculatedOther)
	{
		float LineX1 = (float)(F_Seg.end_x - F_Seg.start_x);
		float LineY1 = float(F_Seg.end_y - F_Seg.start_y);

		float LineX2 = m_Ball->GetBallPosition().x - (float)F_Seg.start_x;
		float LineY2 = m_Ball->GetBallPosition().y - (float)F_Seg.start_y;

		float FrameSegmentLength = LineX1 * LineX1 + LineY1 * LineY1;

		float t = std::max(0.0f, std::min(FrameSegmentLength, (LineX1 * LineX2 + LineY1 * LineY2))) / FrameSegmentLength;

		float ClosestPointX = F_Seg.start_x + t * LineX1;
		float ClosestPointY = F_Seg.start_y + t * LineY1;

		if (!isVectorCalculated)
		{
			isVectorCalculated = true;
			TangentPosX = TangentPosition(F_Seg).x;
			TangentPosY = TangentPosition(F_Seg).y;
			VectorX = - m_Ball->GetVelocity().x;
			VectorY = - m_Ball->GetVelocity().y;
			// std::cout << "VectorX=" << VectorX << std::endl;
			// std::cout << "VectorY=" << VectorY << std::endl;
			// std::cout << "TangentPosition(F_Seg).x1=" << TangentPosX << std::endl;
			// std::cout << "TangentPosition(F_Seg).y1=" << TangentPosY << std::endl;
		}

		float vect_x = m_Ball->GetBallPosition().x - TangentPosX;
		float vect_y = m_Ball->GetBallPosition().y - TangentPosY;
		// std::cout << "----VectorX * vect_x=" << VectorX * vect_x << std::endl;
		// std::cout << "----VectorY * vect_y=" << VectorY * vect_y << std::endl;
		//std::cout << "id=" << F_Seg.frame_id << std::endl;
		// if direction of VectorX && vect_x is different (thus product of these two has to be < 0) and the same for VectorY and vect_y
		if (VectorX * vect_x < 0.0f && VectorY * vect_y < 0.0f && !m_Bounced)
		{
			// std::cout << "===============\n";
			// std::cout << "VectorX * vect_x=" << VectorX * vect_x << std::endl;
			// std::cout << "VectorY * vect_y=" << VectorY * vect_y << std::endl;
			// std::cout << "TangentPosition(F_Seg).x2=" << TangentPosition(F_Seg).x << std::endl;
			// std::cout << "TangentPosition(F_Seg).y2=" << TangentPosition(F_Seg).y << std::endl;

			float distance = Distance(ClosestPointX, ClosestPointY, m_Ball->GetBallPosition().x, m_Ball->GetBallPosition().y);
			
			// Normal unit vector
			float nx = (ClosestPointX - m_Ball->GetBallPosition().x) / distance;
			float ny = (ClosestPointY - m_Ball->GetBallPosition().y) / distance;
			// Tangent unit vector   nx*tx + ny*ty = 0 <- dot product = 0 -> perpendicular
			float tx = -ny;
			float ty = nx; 

			float dpTan1 = m_Ball->GetVelocity(true).x * tx + m_Ball->GetVelocity(true).y * ty;
			float dpTan2 = -dpTan1;

			float dpNorm1 = m_Ball->GetVelocity(true).x * nx + m_Ball->GetVelocity(true).y * ny;
			float dpNorm2 = -dpNorm1;

			float mass1 = 1.0f;
			float mass2 = mass1;
			float m1 = (dpNorm1 * (mass1 - mass2) + 2.0f * mass2 * dpNorm2) / (mass1 + mass2);
			float m2 = (dpNorm1 * (mass2 - mass1) + 2.0f * mass1 * dpNorm1) / (mass1 + mass2);

			m_Ball->SetBallPosition(TangentPosX, TangentPosY);
			m_Ball->SetVelocity(tx * dpTan1 + nx * m1 * 1.1f , ty * dpTan1 + ny * m1 * 1.1f);

			if (F_Seg.frame_id < m_BlockLevels.front().size())
			{
				HitBlock(F_Seg.frame_id);
				std::cout << "Hit!\n";
			}


			isVectorCalculated = false;
			isVectorCalculatedOther = false;
			m_Bounced = true;
		}
	}

	virtual void HitBlock(unsigned int index)
	{
		if (m_BlockLevels.front()[index]->GetBlockDurability() == DURABILITY::TWO_HIT)
		{
			m_BlockLevels.front()[index]->LowerDurability();
		}
		else if (m_BlockLevels.front()[index]->GetBlockDurability() == DURABILITY::ONE_HIT)
		{
			float x = m_BlockLevels.front()[index]->GetPosition().x;
			float y = m_BlockLevels.front()[index]->GetPosition().y;

			auto it = m_BlockLevels.front().begin() + index;
			auto it2 = m_BlockLevels.front().erase(it);
			for (auto i = it2; i != m_BlockLevels.front().end(); i++)
			{
				(*i)->SetId((*i)->GetId() - 1);
			}
			m_Ball->SetVelocity(m_Ball->GetVelocity(true).x * 0.9f / 1.1f , m_Ball->GetVelocity(true).y * 0.9f / 1.1f);
			m_BlockDestroyedAmount++;
			//if (m_BlockDestroyedAmount % 2 == 0)
			SpawnAbility(x, y);
		}
	}

	virtual void SpawnAbility(float x, float y)
	{
		auto RandomAbility = []()
		{
			if (rand() % 2 == 0)
				return ABILITY_TYPE::POSITIVE;
			else
				return ABILITY_TYPE::NEGATIVE;
		};
		m_Abilities.push_back(std::make_unique<Ability>(RandomAbility()));
		m_Abilities.back()->SetPosition(x + m_BlockSize.width / 2, y + m_BlockSize.height / 2);
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative) override
	{	
		m_MousePosition.x = x;
		m_MousePosition.y = y;
		if (!m_GameActive)
			std::cout << m_MousePosition.x << " | " << m_MousePosition.y << std::endl;
		
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) override
	{
		if (!m_GameActive)
		{
			std::cout << "Shoot\n";
			ShootBall();
			m_GameActive = true;
			m_Bounced = true;
		}
	}

	virtual void onKeyPressed(FRKey k) override
	{
		m_KeyPressed = true;
		m_CurrentKey = k;
		std::cout << "onKeyPressed\n";
		
	}

	virtual void onKeyReleased(FRKey k) override
	{
		m_KeyPressed = false;
		std::cout << "onKeyReleased\n";
	}
	
	virtual void RestartGame()
	{

	}

	virtual const char* GetTitle() override
	{
		return "Arcanoid";
	}

	~MyFramework()
	{
		std::cout << "Framework destroyed\n";
		destroySprite(m_BackgroundSprite);
	}

protected:
	unsigned m_WindowWidth;
	unsigned m_WindowHeight;

	// Frame independent movement
	unsigned int m_StartTimePoint;
	unsigned int m_EndTimePoint;
	float m_ElapsedTime;

	// Platform
	std::unique_ptr<Platform> m_Platform;
	unsigned int m_PlatformId;
	FRKey m_CurrentKey;
	FRKey m_PreviousKey;
	bool m_KeyPressed;

	// Mouse
	POINT p;
	coords<int> m_MousePosition;
	std::unique_ptr<Mouse> m_Mouse;

	// Walls
	std::vector<std::unique_ptr<WallElement>> m_WallElements;
	unsigned int m_NumberOfWallElementsWidth;
	unsigned int m_NumberOfWallElementsHeight;
	unsigned int m_NumberOfWallElementsWidthReal;
	unsigned int m_NumberOfWallElementsHeightReal;
	int x_WallOffset;
	int y_Start;
	int x_Start;
	pair<int> m_WallElementSize;
	MapFrame m_MapFrame;
	unsigned int m_MapId;

	// Health
	std::vector<std::unique_ptr<Health>> m_Health;
	unsigned int m_HealthNumber;
	pair<int> m_HealthSize;

	// Levels
	std::queue<std::vector<std::unique_ptr<Block>>> m_BlockLevels;
	pair<int> m_BlockSize;
	unsigned int m_BlockDestroyedAmount;

	// Ball
	std::unique_ptr<Ball> m_Ball;
	bool m_GameActive;

	// Abilities
	std::vector<std::unique_ptr<Ability>> m_Abilities;

	// Physics
	bool m_PossibleCollisionWithPlatform;
	std::vector<std::pair<FrameSegment, coords<float>>> m_IntersectionPoints;
	std::vector<std::pair<FrameSegment, coords<float>>> m_ClosestFrameSegmentsForStaticInstance;
	std::pair<FrameSegment, FrameSegment> m_ClosestStaticFrameSegments;
	float m_StaticVectorX1, m_StaticVectorY1;
	float m_StaticVectorX2, m_StaticVectorY2;
	float m_TangentPosX1, m_TangentPosY1;
	float m_TangentPosX2, m_TangentPosY2;
	float m_PlatformVectorX1, m_PlatformVectorX2;

	bool m_isVectorCalculated1;
	bool m_isVectorCalculated2;
	bool m_Bounced = false;
	bool m_Calculate = false;

	int m_SimulationUpdates = 2;
	float m_SimElapsedTime; 

	// Background
	Sprite* m_BackgroundSprite;
};

std::smatch GetWindowSize(std::string WindowSize)
{
	std::regex Pattern("([0-9]+)[x]([0-9]+)");
	std::smatch Size;
	if (!std::regex_match(WindowSize, Size, Pattern))
	{
		std::cout << "Wrong Window Size! Example:   ./game.exe -window 800x600\n";
	}
	return Size;
}

int main(int argc, char* argv[])
{
	if (argc == 3 && strcmp(argv[1], "-window") == 0)
	{
		auto WindowSize = GetWindowSize(argv[2]);
		if (stoi(WindowSize[0]) >= 600)
			return run(new MyFramework(stoi(WindowSize[0]), stoi(WindowSize[1]))); // seems like a bug - window is always a square with sides equal to width
		else
		{
			std::cout << "Minimum window width is 600!\n";
			return 0;
		}
	}
	else
		return run(new MyFramework);
}
