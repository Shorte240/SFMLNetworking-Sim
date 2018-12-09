// Obstacle Manager class
// Manages all the obstacles

#pragma once

#include "Includes.h"
#include "Obstacle.h"

class ObstacleManager
{
public:
	ObstacleManager(sf::RenderWindow* hwnd, Input* in);
	~ObstacleManager();

	// Update the obstacle manager
	void update(float dt);
	// Render all obstacles this manager has
	void render(sf::RenderWindow* window);
	// Add an obstacle
	void addObstacle(int id, float posX, float posY);

	// Get the vector of obstacles
	std::vector<Obstacle>& getObstacles() { return Obstacles; };
	

private:
	// SFML Variables
	sf::RenderWindow* window;
	Input* input;

	// Place an obstacle where left mouse was clicked
	void placeObstacle();
	// Update the obstacle count text
	void updateText();

	// Text & Font
	sf::Font font;
	sf::Text obstacleCountText;

	// Entity setup
	std::vector<Obstacle> Obstacles;
};

