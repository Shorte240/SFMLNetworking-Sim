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

	void update(float dt);
	void render(sf::RenderWindow* window);

	// Get the list of obstacles
	std::list<Obstacle>& getObstacles() { return Obstacles; };

private:
	// Variables
	sf::RenderWindow* window;
	Input* input;

	// Functions
	void placeObstacle();

	// Entity setup
	std::list<Obstacle> Obstacles;
};

