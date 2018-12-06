// Boid class
// Basic movement and setup

#pragma once

#include "Includes.h"

class Boid : public sf::CircleShape
{
public:
	Boid(int id, sf::Vector2f& pos, sf::Vector2f& vel, sf::Color& col);
	~Boid();

	void update(float dt);

	// Get the Boids velocity
	sf::Vector2f getBoidVelocity() { return velocity; };
	int getBoidID() { return ID; };

	// Set the Boids velocity
	void setBoidVelocity(sf::Vector2f vel);
	void setBoidID(int id) { ID = id; };

protected:
	int ID;
	sf::Vector2f velocity;
	sf::Vector2f position;
};

