#include "ObstacleManager.h"

ObstacleManager::ObstacleManager(sf::RenderWindow* hwnd, Input* in)
{
	// Initialise window and input
	window = hwnd;
	input = in;
}

ObstacleManager::~ObstacleManager()
{
}

void ObstacleManager::update(float dt)
{
	placeObstacle();	
}

void ObstacleManager::render(sf::RenderWindow * window)
{
	// Draw the obstacle
	for (auto& obstacle : Obstacles)
	{
		window->draw(obstacle);
	}
}

// Place an obstacle at the position the left mouse was clicked
void ObstacleManager::placeObstacle()
{
	if (input->isMouseLeftDown())
	{
		input->setMouseLeftDown(false);
		Obstacles.push_back(Obstacle(sf::Vector2f(input->getMouseX(), input->getMouseY())));
	}
}
