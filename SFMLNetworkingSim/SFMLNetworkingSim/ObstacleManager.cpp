#include "ObstacleManager.h"

ObstacleManager::ObstacleManager(sf::RenderWindow* hwnd, Input* in)
{
	// Initialise window and input
	window = hwnd;
	input = in;

	// Set up font
	if (!font.loadFromFile("font/arial.ttf"))
	{
		std::cout << "Font can't load" << std::endl;
	}
}

ObstacleManager::~ObstacleManager()
{
}

void ObstacleManager::update(float dt)
{
	placeObstacle();
	updateText();
}

void ObstacleManager::render(sf::RenderWindow * window)
{
	// Draw the obstacle
	for (auto& obstacle : Obstacles)
	{
		window->draw(obstacle);
	}
	window->draw(obstacleCountText);
}

void ObstacleManager::addObstacle(float posX, float posY)
{
	Obstacles.push_back(Obstacle(sf::Vector2f(posX, posY)));
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

void ObstacleManager::updateText()
{
	// Boid separation value text
	obstacleCountText.setFont(font);
	obstacleCountText.setCharacterSize(12);
	obstacleCountText.setString("Obstacle Count: " + std::to_string(Obstacles.size()));
	obstacleCountText.setFillColor(sf::Color::White);
	obstacleCountText.setPosition(window->getSize().x - 162, 36);
}
