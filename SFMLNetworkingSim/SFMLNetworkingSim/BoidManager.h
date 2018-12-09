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

	// Update the boid manager
	void update(float dt, std::vector<Obstacle>& obs);
	// Render all the boids being managed
	void render(sf::RenderWindow* window);

	// Add a boid to the flock
	void addBoidToFlock(int ID, float posX, float posY, float velX, float velY, int r, int g, int b, int a);
	// Get the vector of boids
	std::vector<Boid>& getBoidFlock() { return boidFlock; };

private:
	// SFML Variables
	sf::RenderWindow* window;
	Input* input;

	// Functions
	// Update the boid count and separation text
	void updateText();
	// Give all the boids randomised positions
	void initialisePositions();
	// Make the boids follow the rules of movement of the simulation
	void moveBoids(float dt, std::vector<Obstacle>& obs);
	// Limit the boids velocity
	void limitVelocity(Boid& b, float dt);
	// Make the boids go to the opposite side of the window if they go out of bounds
	void positionWindowWrapping(Boid& b, float dt);
	// Make the boids move towards the perceived centre of the flock
	sf::Vector2f moveTowardsGroup(Boid& b, float dt);
	// Make the boids check the distance between themselves and other boids
	sf::Vector2f distanceCheck(Boid& b, float dt);
	// Make the boids match other boids velocities
	sf::Vector2f matchVelocity(Boid& b, float dt);
	// Make the boids go towards a specific place
	sf::Vector2f seekPlace(Boid& b, float dt, sf::Vector2f place);
	// Make the boids stay within/move back to the window dimensions
	sf::Vector2f boundPositions(Boid& b, float dt);
	// Make the boids avoid a particular place
	sf::Vector2f avoidPlace(Boid& b, float dt, std::vector<Obstacle>& obs);

	// Text & Font
	sf::Font font;
	sf::Text boidSeparationText;
	sf::Text boidCountText;

	// Entity setup
	std::vector<Boid> boidFlock;
	
	// Boid variables
	float speed;
	int separationValue;
};

