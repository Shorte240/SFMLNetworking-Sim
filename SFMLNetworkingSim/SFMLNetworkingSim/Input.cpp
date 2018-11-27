#include "Input.h"

Input::Input()
{
}


Input::~Input()
{
	delete mouseWorldPos;
	mouseWorldPos = NULL;
}

void Input::setKeyDown(int key)
{
	if (key != -1)
	{
		keys[key] = true;
	}
}

void Input::setKeyUp(int key)
{
	if (key != -1)
	{
		keys[key] = false;
	}
}

bool Input::isKeyDown(int key)
{
	return keys[key];
}

void Input::setMousePosition(int lx, int ly)
{
	mouse.x = lx;
	mouse.y = ly;
}

int Input::getMouseX()
{
	return mouse.x;
}

int Input::getMouseY()
{
	return mouse.y;
}

void Input::setMouseLeftDown(bool _left)
{
	mouse.left = _left;
}

void Input::setMouseMiddleDown(bool _middle)
{
	mouse.middle = _middle;
}

void Input::setMouseRightDown(bool _right)
{
	mouse.right = _right;
}

bool Input::isMouseLeftDown()
{
	return mouse.left;
}

bool Input::isMouseMiddleDown()
{
	return mouse.middle;
}

bool Input::isMouseRightDown()
{
	return mouse.right;
}

void Input::setMouseWorld(sf::Vector2f* v)
{
	mouseWorldPos = v;
}

// Get the location of the mouse based on sf::Mouse
// Using this to help correct view bugs with mouse movement.
sf::Vector2f Input::getMouseWorld()
{
	return *mouseWorldPos;
}
