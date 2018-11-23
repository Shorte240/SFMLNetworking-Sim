// Obstacle class
// Boids will tend to avoid where this is placed

#pragma once

#include "Includes.h"

class Obstacle : public sf::CircleShape
{
public:
	Obstacle(const sf::Vector2f & size = sf::Vector2f(0, 0));
	~Obstacle();

	void update(float dt);

protected:
	sf::Vector2f position;
};

