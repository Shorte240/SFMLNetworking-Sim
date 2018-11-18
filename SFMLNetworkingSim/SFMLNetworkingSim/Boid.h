// Boid class
// Basic movement and setup

#pragma once

#include "Includes.h"

class Boid : public sf::CircleShape
{
public:
	Boid(const sf::Vector2f & size = sf::Vector2f(0, 0));
	~Boid();

	void update(float dt);

	// Getters
	sf::Vector2f getBoidPosition() { return position; };
	sf::Vector2f getBoidVelocity() { return velocity; };

	// Setters
	void setBoidPosition(sf::Vector2f pos);
	void setBoidVelocity(sf::Vector2f vel);

protected:
	sf::Vector2f position;
	sf::Vector2f velocity;
};

