// Boid class
// Basic movement and setup

#pragma once

#include "Includes.h"

class Boid : public sf::CircleShape
{
public:
	Boid(sf::Vector2f& pos, sf::Color& col);
	~Boid();

	void update(float dt);

	// Get the Boids velocity
	sf::Vector2f getBoidVelocity() { return velocity; };

	// Set the Boids velocity
	void setBoidVelocity(sf::Vector2f vel);

protected:
	sf::Vector2f velocity;
};

