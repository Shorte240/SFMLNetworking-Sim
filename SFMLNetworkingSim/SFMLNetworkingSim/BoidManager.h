// Boid Manager class
// Manages all the Boids

#pragma once

#include "Includes.h"
#include "Boid.h"

#define NUM_BOIDS 5

class BoidManager
{
public:
	BoidManager(sf::RenderWindow* hwnd);
	~BoidManager();

	void update(float dt);
	void render(sf::RenderWindow* window);

private:
	// Variables
	sf::RenderWindow* window;

	// Functions
	void initialisePositions();
	void moveBoids(float dt);
	sf::Vector2f rule1(Boid& b);
	sf::Vector2f rule2(Boid& b);
	sf::Vector2f rule3(Boid& b);

	// Entity setup
	std::list<Boid> Boids;
};
