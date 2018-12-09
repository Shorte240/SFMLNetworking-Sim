// Input.h
// Manages all input for the simulation

#pragma once
#include <SFML/Graphics.hpp>

class Input
{
public:
	Input();
	~Input();

	void setKeyDown(int key);
	void setKeyUp(int key);
	bool isKeyDown(int key);

	void setMousePosition(int lx, int ly);
	void setMouseLeftDown(bool _left);
	void setMouseMiddleDown(bool _middle);
	void setMouseRightDown(bool _right);
	bool isMouseLeftDown();
	bool isMouseMiddleDown();
	bool isMouseRightDown();
	int getMouseX();
	int getMouseY();

private:
	struct Mouse
	{
		int x, y;
		bool left = false;
		bool right = false;
		bool middle = false;
	};

	bool keys[256]{ false };
	Mouse mouse;
};

