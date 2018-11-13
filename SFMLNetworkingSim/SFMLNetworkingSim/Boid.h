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

protected:
	sf::Vector2f position;
	sf::Vector2f velocity;
};

