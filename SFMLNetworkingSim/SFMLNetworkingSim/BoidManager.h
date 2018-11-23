// Boid Manager class
// Manages all the Boids

#pragma once

#include "Includes.h"
#include "Boid.h"

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
	void outputText();
	void initialisePositions();
	void moveBoids(float dt);
	sf::Vector2f rule1(Boid& b, float dt);
	sf::Vector2f rule2(Boid& b, float dt);
	sf::Vector2f rule3(Boid& b, float dt);
	sf::Vector2f rule4(Boid& b, float dt);
	sf::Vector2f rule5(Boid& b, float dt);
	sf::Vector2f rule6(Boid& b, float dt);

	// Text & Font
	sf::Font font;
	sf::Text fpsText;
	sf::Text boidSeparationText;

	// Entity setup
	std::list<Boid> Boids;
	int fps;
	float speed;
	int separationValue;
};

