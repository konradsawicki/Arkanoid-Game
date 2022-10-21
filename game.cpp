
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
		if (m_MouseAppeared)
			m_Mouse->Draw(m_MousePosition.x, m_MousePosition.y);
		
		return false;
	}

	virtual void UpdateMousePosition()
	{
		GetCursorPos(&p);
		ScreenToClient(m_hWnd, &p);
		if (p.x < 0 && !m_BlockMousePos)
		{
			m_BlockMousePos = true;
			m_MousePosition.y = p.y;
			m_MousePosition.x = 0;
		}
		else if (p.x > m_WindowWidth && !m_BlockMousePos)
		{
			m_BlockMousePos = true;
			m_MousePosition.y = p.y;
			m_MousePosition.x = m_WindowWidth;
		}

		if (p.y < 0 && !m_BlockMousePos)
		{
			m_BlockMousePos = true;
			m_MousePosition.x = p.x;
			m_MousePosition.y = 0;
		}
		else if (p.y > m_WindowHeight && !m_BlockMousePos)
		{
			m_BlockMousePos = true;
			m_MousePosition.x = p.x;
			m_MousePosition.y = m_WindowHeight;
		}
		else if (p.x > 0 && p.x <= m_WindowWidth && p.y > 0 && p.y <= m_WindowHeight)
		{
			m_BlockMousePos = false;
			m_MouseAppeared = true;
			m_MousePosition = p;
		}
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

	POINT p;
	POINT m_MousePosition;
	bool m_BlockMousePos = true;
	bool m_MouseAppeared = false;
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
