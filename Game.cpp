
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
#include <math.h>
#include <Windows.h>
#include <algorithm>
#include <queue>
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
	FrameSegment width1, width2, height1, height2;
};

class MyFramework : public Framework 
{
public:

	MyFramework(unsigned WindowWidth = 1024, unsigned WindowHeight = 768)
		: m_WindowWidth(WindowWidth), m_WindowHeight(WindowHeight)
	{
		ShowWindow(FindWindowA("ConsoleWindowClass", NULL), false);
	}

	void PreInit(int& width, int& height, bool& fullscreen) override
	{
		width = m_WindowWidth;
		height = m_WindowHeight;
		fullscreen = false;
		
	}

	void InitVariables()
	{
		m_NumberOfWallElementsWidth = 25;
		m_NumberOfWallElementsHeight = 23;
		m_HealthNumber = 3;
		m_KeyPressed = false;
		m_GameActive = false;
		m_PossibleCollisionWithPlatform = false;
		x_Start = 0;
		y_Start = 0;
		m_isVectorCalculated1 = false;
		m_isVectorCalculated2 = false;
		m_BlockDestroyedAmount = 0;
		m_isAdditionalWall = false;
		m_Bounced = false;
		m_Calculate = false;
		m_isAdditionalWallChecked = true;
		m_Abilities.reserve(1000);
	}

	void InitPlatform()
	{
		m_Platform = std::make_unique<Platform>("data/48-Breakout-Tiles.png", (x_Start + (m_NumberOfWallElementsWidthReal + 2) * m_WallElementSize.width) / 2);
		m_PlatformId = m_MapFrame.height2.frame_id + 1;
		m_Platform->SetPlatformId(m_PlatformId);
		
	}

	void InitWallElements()
	{
		
		m_NumberOfWallElementsWidthReal = m_NumberOfWallElementsWidth - 6; // i don't want whole window width with Wall Elements, so i substract 6: 3 per side;
		m_NumberOfWallElementsHeightReal = m_NumberOfWallElementsHeight + 1;
		m_WallElements.reserve(2 * m_NumberOfWallElementsWidthReal + 2 * m_NumberOfWallElementsHeightReal + 1);
		for (int i = 0; i < 2 * m_NumberOfWallElementsHeightReal + 2 * m_NumberOfWallElementsWidthReal; ++i) 
		{
			m_WallElements.push_back(std::make_unique<WallElement>("data/29-Breakout-Tiles.png", 
				m_NumberOfWallElementsWidth, m_NumberOfWallElementsHeight));
		}
		m_WallElementSize.width = m_WallElements[0]->GetWallElementSize().width;
		m_WallElementSize.height = m_WallElements[0]->GetWallElementSize().height;

		// Init map frame
		m_MapFrame.width1.start_x = x_Start + m_WallElementSize.width;
		m_MapFrame.width1.start_y = m_WallElementSize.height;
		m_MapFrame.width1.end_x = m_MapFrame.width1.start_x + m_NumberOfWallElementsWidthReal * m_WallElementSize.width;
		m_MapFrame.width1.end_y = m_MapFrame.width1.start_y;

		m_MapFrame.height1.start_x = m_MapFrame.width1.end_x;
		m_MapFrame.height1.start_y = m_MapFrame.width1.end_y;
		m_MapFrame.height1.end_x = m_MapFrame.height1.start_x;
		m_MapFrame.height1.end_y = m_MapFrame.height1.start_y + m_NumberOfWallElementsHeightReal * m_WallElementSize.height;

		m_MapFrame.height2.start_x = m_MapFrame.width1.start_x;
		m_MapFrame.height2.start_y = m_MapFrame.height1.end_y;
		m_MapFrame.height2.end_x = m_MapFrame.height2.start_x;
		m_MapFrame.height2.end_y = m_MapFrame.width1.start_y;

		m_MapFrame.width2.start_x = m_MapFrame.height2.end_x;
		m_MapFrame.width2.start_y = m_WindowHeight - m_WallElementSize.height;
		m_MapFrame.width2.end_x = m_MapFrame.width1.end_x;
		m_MapFrame.width2.end_y = m_MapFrame.width2.start_y;
	}

	void InitHealth()
	{
		for (int i = 0; i < m_HealthNumber; ++i)
		{
			m_Health.push_back(std::make_unique<Health>("data/60-Breakout-Tiles.png"));
		}
		m_HealthSize.width = m_Health[0]->GetHealthSize().width;
		m_HealthSize.height = m_Health[0]->GetHealthSize().height;
	}

	void InitBlockLevels()
	{
		Block::m_ID = 0;
		unsigned int NumberOfBlocksWidth = 12;
		unsigned int NumberOfBlocksHeight = 25;
		m_BlockSize = {(int)(m_NumberOfWallElementsWidthReal * m_WallElementSize.width / NumberOfBlocksWidth), (int)(m_NumberOfWallElementsHeightReal * m_WallElementSize.height / NumberOfBlocksHeight)};

		BlockType BlockInfHit;
		BlockInfHit.Durability = DURABILITY::INF_HIT;
		BlockInfHit.PathNoHit = "data/09-Breakout-Tiles.png";
		BlockInfHit.PathHit = "data/10-Breakout-Tiles.png";

		auto RandomBlockType = [&]() 
		{
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
		m_MapFrame.width1.frame_id = m_BlockLevels.front().size();
		m_MapFrame.width2.frame_id = m_BlockLevels.front().size() + 1;
		m_MapFrame.height1.frame_id = m_BlockLevels.front().size() + 2;
		m_MapFrame.height2.frame_id = m_BlockLevels.front().size() + 3;
	}

	void InitMouse()
	{
		m_Mouse = std::make_unique<Mouse>("data/59-Breakout-Tiles.png");
		showCursor(false);
	}

	void InitBall()
	{
		m_Ball = std::make_unique<Ball>("data/62-Breakout-Tiles.png", m_Platform->GetPlatformCenter().x, m_Platform->GetPlatformCenter().y);
	}

	void InitBackground()
	{
		m_BackgroundSprite = createSprite("data/Background_space.png");
		setSpriteSize(m_BackgroundSprite, m_WindowWidth, m_WindowHeight);
	}

	void InitTimePoints()
	{
		m_StartTimePoint = getTickCount();
		m_EndTimePoint = getTickCount();
	}

	bool Init() override
	{	
		InitVariables();
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

	void Close() override
	{

	}

	void DrawBackground()
	{
		drawSprite(m_BackgroundSprite, 0, 0);
	}

	void DrawWallElements()
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
			else if (counter >= m_NumberOfWallElementsHeightReal + m_NumberOfWallElementsWidthReal && counter < 2 * m_NumberOfWallElementsHeightReal + m_NumberOfWallElementsWidthReal)
			{
				e->Draw(m_WindowWidth - x_WallOffset - m_WallElementSize.width, y2);
				y2 += m_WallElementSize.height;
				counter++;
			}
			else if (counter >= 2 * m_NumberOfWallElementsHeightReal + m_NumberOfWallElementsWidthReal && m_isAdditionalWall)
			{
				e->Draw(x, m_WindowHeight - m_WallElementSize.height);
				x -= m_WallElementSize.width;
				counter++;
			}
		}
	}

	void DrawBlockLevel()
	{
		if (!m_BlockLevels.empty())
		{		
			for (auto& e : m_BlockLevels.front())
			{
				e->Draw();
			}
		}
	}

	void DrawHealth()
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
					x1 += m_HealthSize.width;
					counter++;
					
				}
				else
				{
					e->Draw(m_WindowWidth - x_WallOffset + x2, m_WindowHeight / 4 + m_HealthSize.height + 10);
					x2 += m_HealthSize.width;
					counter++;
				}
			}
		}
	}

	void DrawPlatform()
	{	
		m_Platform->Draw();
	}

	void DrawBall()
	{
		m_Ball->Draw(m_ElapsedTime);
	}

	void DrawMouse()
	{
		m_Mouse->Draw(m_MousePosition.x, m_MousePosition.y);
	}

	void DrawAbilities()
	{
		if (!m_Abilities.empty())
		{
			for (auto& e : m_Abilities)
			{
				e->Draw();
			}
		}
	}

	bool Tick() override
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


		CheckAdditionalWall();
		CheckCollision();

		RestartGame();
		GoToNextLevel();
		return false;
	}

	void UpdateElapsedTime()
	{
		m_EndTimePoint = getTickCount();
		m_ElapsedTime = float(m_EndTimePoint - m_StartTimePoint);
		m_StartTimePoint = m_EndTimePoint;
		
	}

	void UpdateBallPosition()
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

	void UpdatePlatformPosition()
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
					break;
				default:
					break;
			}
		}
	}

	void UpdateAbilitiesPosition()
	{
		if (!m_Abilities.empty())
		{
			int index = -1;
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
					(*it)->m_SettledAbilities.push_back(&ability);
					ability->Settle(true);
					ability->SetPosition(ability->GetPosition().x + (float)ability->GetSize().width / 2.0f, (*it)->GetPosition().y - ability->GetSize().height / 2);
				}

				if (ability->GetPosition().y + ability->GetSize().height >= m_Platform->GetPlatformCenter().y && 
						ability->GetPosition().y <= m_Platform->GetPlatformCenter().y + m_Platform->GetSize().height &&
							ability->GetPosition().x + ability->GetSize().width >= m_Platform->GetPosition().x &&
								ability->GetPosition().x <= m_Platform->GetPosition().x + m_Platform->GetSize().width)
				{
					index = ability->GetId();
					UseAbility(&ability);
				}
			}
			if (index != -1)
			{
				auto it = m_Abilities.begin() + index;
				auto it2 = m_Abilities.erase(it);
				for (auto i = it2; i != m_Abilities.end(); i++)
				{
					(*i)->SetId((*i)->GetId() - 1);
				}
				Ability::SetGlobalId(Ability::GetGlobalId() - 1);
			}

		} 
	}

	void CheckAdditionalWall()
	{
		if (m_isAdditionalWall && !m_isAdditionalWallChecked)
		{
			m_isAdditionalWallChecked = true;
			m_Bounced = true;
		}
	}

	float Distance(float x1, float y1, float x2, float y2)
	{
		return sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
	}

	coords<float> FindStaticIntersectionPoints(FrameSegment F_Segment, int i)
	{
		float radius = (float)m_Ball->GetBallRadius();
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
					m_IntersectionPoints.push_back({F_Segment, {p_x, p_y}});
				}
				else if (p_x >= (float)F_Segment.end_x && p_x <= (float)F_Segment.start_x)
				{	
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
					m_IntersectionPoints.push_back({F_Segment, {p_x, p_y}});
				}
				else if (p_y >= (float)F_Segment.end_y && p_y <= (float)F_Segment.start_y)
				{
					m_IntersectionPoints.push_back({F_Segment, {p_x, p_y}});
				}
			}
		}
		return {0, 0};
	}

	void FindClosestPointForStaticInstance()
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

	void FindClosestStaticFrameSegments()
	{

		if (!m_ClosestFrameSegmentsForStaticInstance.empty())
		{	
			if (m_ClosestFrameSegmentsForStaticInstance.size() >= 2)
			{
				std::sort(m_ClosestFrameSegmentsForStaticInstance.begin(), m_ClosestFrameSegmentsForStaticInstance.end(), 
				[&](const auto& kv1, const auto& kv2)
				{
					return Distance(kv1.second.x, kv1.second.y, m_Ball->GetBallPosition().x, m_Ball->GetBallPosition().y) < Distance(kv2.second.x, kv2.second.y, m_Ball->GetBallPosition().x, m_Ball->GetBallPosition().y);
				});

				m_ClosestStaticFrameSegments.first = m_ClosestFrameSegmentsForStaticInstance[0].first;
				m_ClosestStaticFrameSegments.second = m_ClosestFrameSegmentsForStaticInstance[1].first;
			}
			else if (m_ClosestFrameSegmentsForStaticInstance.size() == 1)
			{
				m_ClosestStaticFrameSegments.first = m_ClosestFrameSegmentsForStaticInstance[0].first;
				m_ClosestStaticFrameSegments.second = m_ClosestFrameSegmentsForStaticInstance[0].first;
			}

			m_ClosestFrameSegmentsForStaticInstance.clear();
			m_Calculate = true;
		}
		else
		{
			m_Calculate = false;
		}
			
	}

	void CheckCollision()
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
			for (auto& e : m_BlockLevels.front())
			{
				FindStaticIntersectionPoints(e->GetBlockFrame().width1, -1);
				FindStaticIntersectionPoints(e->GetBlockFrame().height1, -1);
				FindStaticIntersectionPoints(e->GetBlockFrame().width2, -1);
				FindStaticIntersectionPoints(e->GetBlockFrame().height2, -1);
			}
			FindClosestPointForStaticInstance();

			// Collision with walls
			FindStaticIntersectionPoints(m_MapFrame.width1, 1);
			FindStaticIntersectionPoints(m_MapFrame.height1, 1);
			FindStaticIntersectionPoints(m_MapFrame.height2, 1);
			if (m_isAdditionalWall)
				FindStaticIntersectionPoints(m_MapFrame.width2, 1);
			FindClosestPointForStaticInstance();

			FindStaticIntersectionPoints(m_MapFrame.width1, -1);
			FindStaticIntersectionPoints(m_MapFrame.height1, -1);
			FindStaticIntersectionPoints(m_MapFrame.height2, -1);
			if (m_isAdditionalWall)
				FindStaticIntersectionPoints(m_MapFrame.width2, -1);
			FindClosestPointForStaticInstance();

			// Find overall two closest static frames
			FindClosestStaticFrameSegments();
			m_Bounced = false;
		}
	}

	void ShootBall()
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

	coords<float> TangentPosition(FrameSegment F_Seg)
	{
		float a = m_Ball->GetPathCoefficients().a;
		float b = m_Ball->GetPathCoefficients().b;
		float radius = m_Ball->GetBallRadius();
		float tanpos_x, tanpos_y;
		
		if (F_Seg.start_y == F_Seg.end_y)
		{
			if ((float)F_Seg.start_y < m_Ball->GetBallPosition().y)
				tanpos_y = (float)(F_Seg.start_y + radius);
			else if ((float)F_Seg.start_y > m_Ball->GetBallPosition().y)
				tanpos_y = (float)(F_Seg.start_y - radius);
			else
				tanpos_y = (float)F_Seg.start_y;

			tanpos_x = (tanpos_y - b) / a;
		}
		else if (F_Seg.start_x == F_Seg.end_x)
		{
			if ((float)F_Seg.start_x < m_Ball->GetBallPosition().x)
				tanpos_x = (float)(F_Seg.start_x + radius);
			else if ((float)F_Seg.start_x > m_Ball->GetBallPosition().x)
				tanpos_x = (float)(F_Seg.start_x - radius);
			else
				tanpos_x = (float)F_Seg.start_x;
			tanpos_y = a * tanpos_x + b;
		}
		return {tanpos_x, tanpos_y};
	}

	void DynamicResponsePlatform(FrameSegment F_Seg)
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
			float mass2 = mass1;
			float m1 = (dpNorm1 * (mass1 - mass2) + 2.0f * mass2 * dpNorm2) / (mass1 + mass2);
			float m2 = (dpNorm1 * (mass2 - mass1) + 2.0f * mass1 * dpNorm1) / (mass1 + mass2);

			float overlap = distance - m_Ball->GetBallRadius() - F_Seg.radius;
			m_Ball->SetBallPosition(m_Ball->GetBallPosition().x + overlap * nx, m_Ball->GetBallPosition().y + overlap * ny);
			m_Ball->SetVelocity(tx * dpTan1 + nx * m1, ty * dpTan1 + ny * m1);

			m_Bounced = true;

			m_isVectorCalculated1 = false;
			m_isVectorCalculated2 = false;
		}
	}

	void DynamicResponseStatic(FrameSegment F_Seg, float& VectorX, float& VectorY, 
		float& TangentPosX, float& TangentPosY, bool& isVectorCalculated, bool& isVectorCalculatedOther)
	{
		
		float LineX1 = (float)(F_Seg.end_x - F_Seg.start_x);
		float LineY1 = float(F_Seg.end_y - F_Seg.start_y);

		float LineX2 = m_Ball->GetBallPosition().x - (float)F_Seg.start_x;
		float LineY2 = m_Ball->GetBallPosition().y - (float)F_Seg.start_y;

		float FrameSegmentLength = LineX1 * LineX1 + LineY1 * LineY1;

		float t = std::max(0.0f, std::min(FrameSegmentLength, (LineX1 * LineX2 + LineY1 * LineY2))) / FrameSegmentLength;

		float ClosestPointX = (float)F_Seg.start_x + t * LineX1;
		float ClosestPointY = (float)F_Seg.start_y + t * LineY1;

		if (!isVectorCalculated)
		{
			isVectorCalculated = true;
			TangentPosX = TangentPosition(F_Seg).x; // kiedy leci na rog to nie jest styczny
			TangentPosY = TangentPosition(F_Seg).y;
			VectorX = m_Ball->GetBallPosition().x - TangentPosX; // reference vectors
			VectorY = m_Ball->GetBallPosition().y - TangentPosY;
		}

		float vect_x = m_Ball->GetBallPosition().x - TangentPosX;
		float vect_y = m_Ball->GetBallPosition().y - TangentPosY;
		// if directions of reference vector VectorX/Y and live vector vect_x/y are different, 
		// then the dot product of these two has to be < 0 (theoretically, because when I had < 0.0f there were bugs,
		// and with < 5.0f is much better (it means that they are really close and the directions of vect_x and y are about to switch)
		// I am also making it dependent of velocity
		if (VectorX * vect_x < std::max(5.0f, 5.0f * abs(m_Ball->GetVelocity().x)) && VectorY * vect_y < std::max(5.0f, 5.0f * abs(m_Ball->GetVelocity().y)) && !m_Bounced)
		{
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
			float m2 = (dpNorm2 * (mass2 - mass1) + 2.0f * mass1 * dpNorm1) / (mass1 + mass2);



			m_Ball->SetVelocity(tx * dpTan1 + nx * m1 * 1.1f, ty * dpTan1 + ny * m1 * 1.1f);


			// when t=0 or t=FrameSegmentLength the position isnt TangentPosX and TangentPosY, 
			// but it's quite good aproximation, and the physics model seems to behave properl
			m_Ball->SetBallPosition(TangentPosX, TangentPosY);

			if (F_Seg.frame_id < m_BlockLevels.front().size())
			{
				HitBlock(F_Seg.frame_id);
			}
			else if (F_Seg.frame_id == m_MapFrame.width2.frame_id)
			{
				RemoveWall();
			}
				
			isVectorCalculated = false;
			isVectorCalculatedOther = false;
			m_Bounced = true;
		}
	}

	void HitBlock(unsigned int index)
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
			m_Ball->SetVelocity(m_Ball->GetVelocity(true).x * 0.9f / 1.1f, m_Ball->GetVelocity(true).y * 0.9f / 1.1f);
			m_BlockDestroyedAmount++;
			if (m_BlockDestroyedAmount % 3 == 0) // or (m_BlockDestroyedAmount % 10 == 0) but it is too rarely for me
				SpawnAbility(x, y);
		}
	}

	void SpawnAbility(float x, float y)
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

	void UseAbility(std::unique_ptr<Ability>* ability_ptr)
	{
		if ((*ability_ptr)->GetType() == ABILITY_TYPE::POSITIVE)
		{
			AddWall();
		}
		else if ((*ability_ptr)->GetType() == ABILITY_TYPE::NEGATIVE)
			m_Health.pop_back();
	}

	void AddWall()
	{
		if (!m_isAdditionalWall)
		{
			m_isAdditionalWall = true;
			m_isAdditionalWallChecked = false;
		}
	}

	void RemoveWall()
	{
		m_isAdditionalWall = false;
	}

	void onMouseMove(int x, int y, int xrelative, int yrelative) override
	{	
		m_MousePosition.x = x;
		m_MousePosition.y = y;	
	}

	void onMouseButtonClick(FRMouseButton button, bool isReleased) override
	{
		if (!m_GameActive)
		{
			std::cout << "Shoot\n";
			ShootBall();
			m_GameActive = true;
			m_Bounced = true;
		}
	}

	void onKeyPressed(FRKey k) override
	{
		m_KeyPressed = true;
		m_CurrentKey = k;
	}

	void onKeyReleased(FRKey k) override
	{
		m_KeyPressed = false;
	}
	
	void RestartGame()
	{
		if (m_Health.empty() || m_Ball->GetBallPosition().y > m_WindowHeight || // <- real constraints
				m_Ball->GetBallPosition().y < 0 || m_Ball->GetBallPosition().x > m_WindowWidth || m_Ball->GetBallPosition().x < 0)
				// ^ these conditions added due to bugs, they wouldnt be there in completed bug-free game ^
		{
			m_Platform = nullptr;
			m_Mouse = nullptr;
			m_WallElements.clear();
			while (!m_BlockLevels.empty())
			{
				m_BlockLevels.pop();
			}
			m_Ball = nullptr;
			m_Health.clear();
			m_Abilities.clear();
			Ability::SetGlobalId(0);
			m_IntersectionPoints.clear();
			m_ClosestFrameSegmentsForStaticInstance.clear();

			Init();

			if (m_Ball->GetBallPosition().y < 0 || m_Ball->GetBallPosition().x > m_WindowWidth || m_Ball->GetBallPosition().x < 0)
				std::cout << "Oops! Bug occured :(\n";
		}
	}

	void GoToNextLevel()
	{
		if (!m_BlockLevels.empty())
		{
			if (std::all_of(m_BlockLevels.front().begin(), m_BlockLevels.front().end(), [&](const auto& block)
			{
				return block->GetBlockDurability() == DURABILITY::INF_HIT;
			}))
			{
				m_BlockLevels.pop();
				m_Abilities.clear();
				Ability::SetGlobalId(0);
				InitVariables();
				m_Ball = nullptr;
				InitBall();
				m_Health.push_back(std::make_unique<Health>("data/60-Breakout-Tiles.png"));

				m_MapFrame.width1.frame_id = m_BlockLevels.front().size();
				m_MapFrame.width2.frame_id = m_BlockLevels.front().size() + 1;
				m_MapFrame.height1.frame_id = m_BlockLevels.front().size() + 2;
				m_MapFrame.height2.frame_id = m_BlockLevels.front().size() + 3;

				m_PlatformId = m_MapFrame.height2.frame_id + 1;
				m_Platform->SetPlatformId(m_PlatformId);
			}
		}
		else
			RestartGame();
	}

	const char* GetTitle() override
	{
		return "Arcanoid";
	}

	~MyFramework()
	{
		std::cout << "Framework destroyed\n";
		destroySprite(m_BackgroundSprite);
		FreeConsole();
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
	unsigned int m_BlockDestroyedAmount;

	// Ball
	std::unique_ptr<Ball> m_Ball;
	bool m_GameActive;

	// Abilities
	std::vector<std::unique_ptr<Ability>> m_Abilities;
	bool m_isAdditionalWall;

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
	bool m_Bounced;
	bool m_Calculate;
	bool m_isVectorCalculated1;
	bool m_isVectorCalculated2;
	bool m_isAdditionalWallChecked;


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
