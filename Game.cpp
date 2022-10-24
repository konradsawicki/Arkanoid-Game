
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


float Distance(int x1, int y1, int x2, int y2)
{
	return sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

class MyFramework : public Framework 
{
public:

	MyFramework(unsigned WindowWidth = 1024, unsigned WindowHeight = 768)
		: m_WindowWidth(WindowWidth), m_WindowHeight(WindowHeight),
			m_NumberOfWallElementsWidth(25), m_NumberOfWallElementsHeight(23),
				m_HealthNumber(3), m_KeyPressed(false), m_GameActive(false),
					m_ClosestIntersectionPointToBall{0.0f, 0.0f}
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
		std::cout << "width1_st_x=" << m_BlockLevels.front()[48]->GetBlockFrame().width1.start_x << std::endl;
		std::cout << "width1_st_y=" << m_BlockLevels.front()[48]->GetBlockFrame().width1.start_y << std::endl;
		std::cout << "width1_end_x=" << m_BlockLevels.front()[48]->GetBlockFrame().width1.end_x << std::endl;
		std::cout << "width1_end_y=" << m_BlockLevels.front()[48]->GetBlockFrame().width1.end_y << std::endl;
		std::cout << "===========================\n";
		std::cout << "height1_st_x=" << m_BlockLevels.front()[48]->GetBlockFrame().height1.start_x << std::endl;
		std::cout << "height1_st_y=" << m_BlockLevels.front()[48]->GetBlockFrame().height1.start_y << std::endl;
		std::cout << "height1_end_x=" << m_BlockLevels.front()[48]->GetBlockFrame().height1.end_x << std::endl;
		std::cout << "height1_end_y=" << m_BlockLevels.front()[48]->GetBlockFrame().height1.end_y << std::endl;
		std::cout << "===========================\n";
		std::cout << "width2_st_x=" << m_BlockLevels.front()[48]->GetBlockFrame().width2.start_x << std::endl;
		std::cout << "width2_st_y=" << m_BlockLevels.front()[48]->GetBlockFrame().width2.start_y << std::endl;
		std::cout << "width2_end_x=" << m_BlockLevels.front()[48]->GetBlockFrame().width2.end_x << std::endl;
		std::cout << "width2_end_y=" << m_BlockLevels.front()[48]->GetBlockFrame().width2.end_y << std::endl;
		std::cout << "===========================\n";
		std::cout << "height2_st_x=" << m_BlockLevels.front()[48]->GetBlockFrame().height2.start_x << std::endl;
		std::cout << "height2_st_y=" << m_BlockLevels.front()[48]->GetBlockFrame().height2.start_y << std::endl;
		std::cout << "height2_end_x=" << m_BlockLevels.front()[48]->GetBlockFrame().height2.end_x << std::endl;
		std::cout << "height2_end_y=" << m_BlockLevels.front()[48]->GetBlockFrame().height2.end_y << std::endl;
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
		InitPlatform();
		InitHealth();
		InitMouse();
		InitBlockLevels();
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
		y_Start = 0;
		int y1 = y_Start;
		int y2 = y_Start;
		int x = 0; // x = x_WallOffset;
		for (auto& e : m_WallElements)
		{	
			if (counter < m_NumberOfWallElementsHeightReal)
			{
				e->Draw(0, y1);
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
					m_Platform->MoveLeft(m_ElapsedTime, m_WallElements[0]->GetWallElementSize().width, x_WallOffset);
					break;
				case FRKey::RIGHT:
					m_Platform->MoveRight(m_ElapsedTime, m_WallElements[0]->GetWallElementSize().width, x_WallOffset);
					if (m_BlockLevels.size() > 1)
						m_BlockLevels.pop();
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

	virtual coords<float> FindIntersectionPoints(BlockFrameSegment BF_Segment)
	{
		for (float i : {1.0f, -1.0f}) // making offset in first iteration + and in second -
		{
			float offset = (float)m_Ball->GetBallRadius() * sinf(PI / 2 - atanf(m_Ball->GetPathCoefficients().a)) * i;
			//std::cout << "offset=" << offset << std::endl;
			if (BF_Segment.start_y == BF_Segment.end_y)
			{
				float p_y = (float)BF_Segment.start_y;
				float p_x = (p_y - m_Ball->GetPathCoefficients().b + offset) / m_Ball->GetPathCoefficients().a;
				count++;
				if (p_x >= (float)BF_Segment.start_x && p_x <= (float)BF_Segment.end_x)
				{
					if (std::none_of(m_IntersectionPoints.begin(), m_IntersectionPoints.end(),
					[&](const auto& kv){
							return (kv.first == BF_Segment.block_id);
					}))
					{
						std::cout << "id=" << BF_Segment.block_id << std::endl;
						std::cout << "p_x=" << p_x << " p_y=" << p_y << std::endl;
						m_IntersectionPoints.push_back({BF_Segment.block_id, {p_x, p_y}});
					}
					
				}
				else if (p_x >= (float)BF_Segment.end_x && p_x <= (float)BF_Segment.start_x)
				{	
					if (std::none_of(m_IntersectionPoints.begin(), m_IntersectionPoints.end(),
					[&](const auto& kv){
							return (kv.first == BF_Segment.block_id);
					}))
					{
						std::cout << "id=" << BF_Segment.block_id << std::endl;
						std::cout << "p_x=" << p_x << " p_y=" << p_y << std::endl;
						m_IntersectionPoints.push_back({BF_Segment.block_id, {p_x, p_y}});
					}
				}
			}
			else if (BF_Segment.start_x == BF_Segment.end_x)
			{
				float p_x = (float)BF_Segment.start_x;
				float p_y = m_Ball->GetPathCoefficients().a * p_x + m_Ball->GetPathCoefficients().b + offset;
				count++;
				if (p_y >= (float)BF_Segment.start_y && p_y <= (float)BF_Segment.end_y)
				{
					if (std::none_of(m_IntersectionPoints.begin(), m_IntersectionPoints.end(),
					[&](const auto& kv){
							return (kv.first == BF_Segment.block_id);
					}))
					{
						std::cout << "id=" << BF_Segment.block_id << std::endl;
						std::cout << "p_x=" << p_x << " p_y=" << p_y << std::endl;
						m_IntersectionPoints.push_back({BF_Segment.block_id, {p_x, p_y}});
					}
				}
				else if (p_y >= (float)BF_Segment.end_y && p_y <= (float)BF_Segment.start_y)
				{
					if (std::none_of(m_IntersectionPoints.begin(), m_IntersectionPoints.end(),
					[&](const auto& kv){
							return (kv.first == BF_Segment.block_id);
					}))
					{
						std::cout << "id=" << BF_Segment.block_id << std::endl;
						std::cout << "p_x=" << p_x << " p_y=" << p_y << std::endl;
						m_IntersectionPoints.push_back({BF_Segment.block_id, {p_x, p_y}});
					}
				}
			}
		}
		return {0, 0};
	}

	virtual void FindClosestIntersectionPointToBall()
	{
		if (!m_IntersectionPoints.empty())
		{		
			std::sort(m_IntersectionPoints.begin(), m_IntersectionPoints.end(), 
			[&](const auto& kv1, const auto& kv2)
			{
				return Distance(kv1.second.x, kv1.second.y, m_Ball->GetBallPosition().x, m_Ball->GetBallPosition().y) < Distance(kv2.second.x, kv2.second.y, m_Ball->GetBallPosition().x, m_Ball->GetBallPosition().y);
			});
			m_ClosestIntersectionPointToBall = m_IntersectionPoints.front().second;
			//std::cout  << "m_IntersectionPoints.size()="<< m_IntersectionPoints.size() << std::endl;
			m_IntersectionPoints.clear();
			std::cout << m_ClosestIntersectionPointToBall.x << " " << m_ClosestIntersectionPointToBall.y << std::endl;
		}
	}

	virtual void CheckCollision()
	{
		if (m_Clicked)
		{
			for (auto& e : m_BlockLevels.front())
			{
				FindIntersectionPoints(e->GetBlockFrame().width1);
				FindIntersectionPoints(e->GetBlockFrame().height1);
				FindIntersectionPoints(e->GetBlockFrame().width2);
				FindIntersectionPoints(e->GetBlockFrame().height2);
			}
			FindClosestIntersectionPointToBall();
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
	pair<int> m_WallElementSize;

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
	std::vector<std::pair<unsigned int, coords<float>>> m_IntersectionPoints;
	coords<float> m_ClosestIntersectionPointToBall;
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
