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

	void update(float dt, std::list<Obstacle>& obs);
	void render(sf::RenderWindow* window);

	void addBoidToFlock(float posX, float posY, float velX, float velY);
	// Get the list of boids
	std::vector<Boid>& getBoidFlock() { return boidFlock; };

private:
	// Variables
	sf::RenderWindow* window;
	Input* input;

	// Functions
	void updateText();
	void initialisePositions();
	void moveBoids(float dt, std::list<Obstacle>& obs);
	void limitVelocity(Boid& b, float dt);
	void positionWindowWrapping(Boid& b, float dt);
	sf::Vector2f moveTowardsGroup(Boid& b, float dt);
	sf::Vector2f distanceCheck(Boid& b, float dt);
	sf::Vector2f matchVelocity(Boid& b, float dt);
	sf::Vector2f seekPlace(Boid& b, float dt, sf::Vector2f place);
	sf::Vector2f boundPositions(Boid& b, float dt);
	sf::Vector2f avoidPlace(Boid& b, float dt, std::list<Obstacle>& obs);

	// Text & Font
	sf::Font font;
	sf::Text boidSeparationText;

	// Entity setup
	std::vector<Boid> boidFlock;
	
	float speed;
	int separationValue;
};

