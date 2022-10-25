
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
#include <regex>
#include <Windows.h>
#include <math.h>
#include <functional>
#include <queue>
#include <thread>
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
					m_PossibleCollisionWithPlatform(false), x_Start(0), y_Start(0)
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
			int b_type = (rand() % 2) + 1;
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
		for (int y = m_WindowHeight / 6; y < m_WindowHeight / 3; y += m_BlockSize.height)
		{	
			int counter = 0;
			for (int x = m_WallElementSize.width + BlockOffset; x < m_NumberOfWallElementsWidthReal * m_WallElementSize.width; x += m_BlockSize.width)
			{
				counter++;

				if (counter % 3 == 0)
					TempBlock = BlockInfHit;
				else
					TempBlock = RandomBlockType();
				Level.push_back(std::make_unique<Block>(TempBlock, m_BlockSize.width, 
						m_BlockSize.height, x, y));
			}
		}
		m_BlockLevels.push(std::move(Level));

		m_MapFrame.width.frame_id = m_BlockLevels.front().size();
		m_MapFrame.height1.frame_id = m_BlockLevels.front().size();
		m_MapFrame.height2.frame_id = m_BlockLevels.front().size();

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

		// Level 2
		for (int counter = 0, y = m_WallElementSize.height; y < m_WindowHeight / 2; y += m_BlockSize.height, ++counter)
		{
			int counter2 = 0;
			for (int x = m_WallElementSize.width; x < m_NumberOfWallElementsWidthReal * m_WallElementSize.width; x += m_BlockSize.width)
			{
				if (y + m_BlockSize.height > m_WindowHeight / 2 && counter2 % 2 == 0)
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

	virtual bool Tick() override
	{	
		UpdateElapsedTime();
		UpdatePlatformPosition();
		UpdateBallPosition();

		DrawBackground();
		DrawBlockLevel();
		DrawWallElements();
		DrawPlatform();
		DrawHealth();
		DrawBall();
		DrawMouse();

		CheckCollision();

		return false;
	}

	virtual void UpdateElapsedTime()
	{
		m_EndTimePoint = getTickCount();
		m_ElapsedTime = m_EndTimePoint - m_StartTimePoint;
		m_StartTimePoint = m_EndTimePoint;
	}

	virtual void UpdateBallPosition()
	{	
		if (!m_GameActive)
			m_Ball->SetBallPosition(m_Platform->GetPlatformCenter().x, m_Platform->GetPlatformCenter().y - m_Ball->GetBallRadius());
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

	virtual void DotProduct(int x1, int y1, int x2, int y2)
	{

	}

	virtual float Distance(int x1, int y1, int x2, int y2)
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
					std::cout << "id=" << F_Segment.frame_id << std::endl;
					std::cout << "p_x=" << p_x << " p_y=" << p_y << std::endl;
					m_IntersectionPoints.push_back({F_Segment.frame_id, {p_x, p_y}});
					
				}
				else if (p_x >= (float)F_Segment.end_x && p_x <= (float)F_Segment.start_x)
				{	
					std::cout << "id=" << F_Segment.frame_id << std::endl;
					std::cout << "p_x=" << p_x << " p_y=" << p_y << std::endl;
					m_IntersectionPoints.push_back({F_Segment.frame_id, {p_x, p_y}});
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
					std::cout << "id=" << F_Segment.frame_id << std::endl;
					std::cout << "p_x=" << p_x << " p_y=" << p_y << std::endl;
					m_IntersectionPoints.push_back({F_Segment.frame_id, {p_x, p_y}});
				}
				else if (p_y >= (float)F_Segment.end_y && p_y <= (float)F_Segment.start_y)
				{
					std::cout << "id=" << F_Segment.frame_id << std::endl;
					std::cout << "p_x=" << p_x << " p_y=" << p_y << std::endl;
					m_IntersectionPoints.push_back({F_Segment.frame_id, {p_x, p_y}});
				}
			}
		}
		return {0, 0};
	}

	virtual void CheckIfPlatformCollides(int i)
	{
		float radius = m_Ball->GetBallRadius();
		float a = m_Ball->GetPathCoefficients().a;
		float b = m_Ball->GetPathCoefficients().b;

		float ball_x = m_Ball->GetBallPosition().x;
		float ball_y = m_Ball->GetBallPosition().y;

		float ball_vel_x = m_Ball->GetVelocity().x;
		float ball_vel_y = m_Ball->GetVelocity().y;

		float offset_x = radius * cosf(PI / 2 - atanf(a)) * (float)i;
		float offset_y = radius * sinf(PI / 2 - atanf(a)) * (float)i;

		float b_p = ball_y + offset_y - a * (ball_x + offset_x);

		float p_y = (float)m_Platform->GetPlatformCenter().y;
		float p_x = (p_y - b_p) / a;

		if (Distance(p_x, p_y, ball_x + offset_x, ball_y + offset_y) > Distance(p_x, p_y,  ball_x + offset_x + ball_vel_x, ball_y + offset_y + ball_vel_y))
		{
			if (p_x >= (float)(x_Start + m_WallElementSize.width) && p_x <= (float)(x_Start + m_WallElementSize.width + m_NumberOfWallElementsWidthReal * m_WallElementSize.width))
				m_PossibleCollisionWithPlatform = true;
		}
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
			m_ClosestPointsForStaticInstance.push_back(m_IntersectionPoints.front());

			m_IntersectionPoints.clear();
		}
	}

	virtual void FindClosestStaticFramesId()
	{
		
		if (!m_ClosestPointsForStaticInstance.empty())
		{	
			if (m_ClosestPointsForStaticInstance.size() > 2)
			{
				std::sort(m_ClosestPointsForStaticInstance.begin(), m_ClosestPointsForStaticInstance.end(), 
				[&](const auto& kv1, const auto& kv2)
				{
					return Distance(kv1.second.x, kv1.second.y, m_Ball->GetBallPosition().x, m_Ball->GetBallPosition().y) < Distance(kv2.second.x, kv2.second.y, m_Ball->GetBallPosition().x, m_Ball->GetBallPosition().y);
				});
			}
			m_ClosestStaticFramesId.first = m_ClosestPointsForStaticInstance[0].first;
			m_ClosestStaticFramesId.second = m_ClosestPointsForStaticInstance[1].first;
			m_ClosestPointsForStaticInstance.clear();

			std::cout << "-----------\n";
			std::cout << "id1=" << m_ClosestStaticFramesId.first <<  std::endl;
			std::cout << "id1_x=" << m_ClosestPointsForStaticInstance[0].second.x << " ";
			std::cout << "id1_y=" << m_ClosestPointsForStaticInstance[0].second.y << std::endl;
			std::cout << "-----------\n";
			std::cout << "id2=" <<m_ClosestStaticFramesId.second << std::endl;
			std::cout << "id2_x=" << m_ClosestPointsForStaticInstance[1].second.x << " ";
			std::cout << "id2_y=" << m_ClosestPointsForStaticInstance[1].second.y << std::endl;
			std::cout << "-----------\n";
		}
	}

	virtual void CheckCollision()
	{
		if (m_Clicked)
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

			// Collision with platform
			CheckIfPlatformCollides(1);
			CheckIfPlatformCollides(-1);

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
			FindClosestStaticFramesId();

			m_Clicked = false;
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

		m_Ball->SetVelocity(v_x, v_y);
	}

	virtual void CalculateTangentPoint()
	{

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
			m_Clicked = true;
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

	// Health
	std::vector<std::unique_ptr<Health>> m_Health;
	unsigned int m_HealthNumber;
	pair<int> m_HealthSize;

	// Levels
	std::queue<std::vector<std::unique_ptr<Block>>> m_BlockLevels;
	pair<int> m_BlockSize;

	// Ball
	std::unique_ptr<Ball> m_Ball;
	bool m_GameActive;

	// Physics
	bool m_PossibleCollisionWithPlatform;
	std::vector<std::pair<unsigned int, coords<float>>> m_IntersectionPoints;
	std::vector<std::pair<unsigned int, coords<float>>> m_ClosestPointsForStaticInstance;
	std::pair<unsigned int, unsigned int> m_ClosestStaticFramesId;
	bool m_Clicked = false;
	int count = 0;

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
