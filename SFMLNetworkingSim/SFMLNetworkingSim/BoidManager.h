// Boid Manager class
// Manages all the Boids

#pragma once

#include "Includes.h"
#include "Boid.h"
#include "Obstacle.h"

#define NUM_BOIDS 5

class BoidManager
{
public:
	BoidManager(sf::RenderWindow* hwnd, Input* in);
	~BoidManager();

	void update(float dt);
	void render(sf::RenderWindow* window);

private:
	// Variables
	sf::RenderWindow* window;
	Input* input;

	// Functions
	void updateText();
	void initialisePositions();
	void moveBoids(float dt);
	void limitVelocity(Boid& b, float dt);
	sf::Vector2f moveTowardsGroup(Boid& b, float dt);
	sf::Vector2f distanceCheck(Boid& b, float dt);
	sf::Vector2f matchVelocity(Boid& b, float dt);
	sf::Vector2f seekPlace(Boid& b, float dt, sf::Vector2f place);
	sf::Vector2f boundPositions(Boid& b, float dt);
	sf::Vector2f avoidPlace(Boid& b, float dt);

	// Text & Font
	sf::Font font;
	sf::Text boidSeparationText;

	// Entity setup
	std::list<Boid> Boids;
	std::list<Obstacle> Obstacles;
	
	float speed;
	int separationValue;
};

