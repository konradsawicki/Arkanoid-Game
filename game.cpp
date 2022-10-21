
#define _WINDOWS
#include "Framework.h"
#include "Platform.h"
#include "Mouse.h"
#include <regex>
#include <Windows.h>
#include <string>
#include <iostream>

/* Test Framework realization */
class MyFramework : public Framework 
{
public:

	MyFramework(unsigned WindowWidth = 1024, unsigned WindowHeight = 768)
		: m_WindowWidth(WindowWidth), m_WindowHeight(WindowHeight)
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
		m_Platform = std::make_unique<Platform>("data/50-Breakout-Tiles.png");
	}

	virtual void InitMouse()
	{
		m_Mouse = std::make_unique<Mouse>("data/59-Breakout-Tiles.png");

		LPCTSTR lpWindowTitle = GetTitle();
		m_hWnd = FindWindow(NULL, lpWindowTitle);

		showCursor(false);
	}

	virtual bool Init() override
	{	
		InitPlatform();
		InitMouse();
		return true;
	}

	virtual void Close() override
	{

	}



	virtual bool Tick() override
	{
        drawTestBackground();
		UpdateMousePosition();
		m_Platform->Draw(100, 100);
		m_Mouse->Draw(m_MousePosition.x, m_MousePosition.y);
		
		return false;
	}

	virtual void UpdateMousePosition()
	{
		GetCursorPos(&m_MousePosition);
		ScreenToClient(m_hWnd, &m_MousePosition);
		if (m_MousePosition.x < 0)
		{
			if (!m_BlockMousePosY)
			{
				m_TempMousePosY = m_MousePosition.y;
				m_BlockMousePosY = true;
			}
			m_MousePosition.x = 0;
			m_MousePosition.y = m_TempMousePosY;
		}
		else if (m_MousePosition.x > m_WindowWidth)
		{
			if (!m_BlockMousePosY)
			{
				m_TempMousePosY = m_MousePosition.y;
				m_BlockMousePosY = true;
			}
			m_MousePosition.x = m_WindowWidth;
			m_MousePosition.y = m_TempMousePosY;
		}
		else
			m_BlockMousePosY = false;


		if (m_MousePosition.y < 0)
		{
			if (!m_BlockMousePosX)
			{
				m_TempMousePosX = m_MousePosition.x;
				m_BlockMousePosX = true;
			}
			m_MousePosition.y = 0;
			m_MousePosition.x = m_TempMousePosX;
		}
		else if (m_MousePosition.y > m_WindowHeight)
		{
			if (!m_BlockMousePosX)
			{
				m_TempMousePosX = m_MousePosition.x;
				m_BlockMousePosX = true;
			}
			m_MousePosition.y = m_WindowHeight;
			m_MousePosition.x = m_TempMousePosX;
		}
		else
			m_BlockMousePosX = false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative) override
	{
		
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) override
	{

	}

	virtual void onKeyPressed(FRKey k) override
	{

	}

	virtual void onKeyReleased(FRKey k) override
	{

	}
	
	virtual const char* GetTitle() override
	{
		return "Arcanoid";
	}

	~MyFramework()
	{

	}

protected:
	unsigned m_WindowWidth;
	unsigned m_WindowHeight;

	std::unique_ptr<Platform> m_Platform;

	POINT m_MousePosition;
	bool m_BlockMousePosX = false;
	bool m_BlockMousePosY = false;
	int m_TempMousePosX;
	int m_TempMousePosY;
	HWND m_hWnd;
	std::unique_ptr<Mouse> m_Mouse;
};

std::smatch GetWindowSize(std::string WindowSize)
{
	std::regex Pattern("([0-9]+)[x]([0-9]+)");
	std::smatch Size;
	if (!std::regex_match(WindowSize, Size, Pattern))
		std::cout << "Wrong Window Size! Example:   ./game.exe -window 800x600\n";
	return Size;
}

int main(int argc, char* argv[])
{
	if (argc == 3 && strcmp(argv[1], "-window") == 0)
	{
		auto WindowSize = GetWindowSize(argv[2]);
		return run(new MyFramework(stoi(WindowSize[0]), stoi(WindowSize[1])));
	}
	else
		return run(new MyFramework);
}
