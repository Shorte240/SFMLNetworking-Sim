// Obstacle class
// Boids will tend to avoid where this is placed

#pragma once

#include "Includes.h"

class Obstacle : public sf::CircleShape
{
public:
	Obstacle(sf::Vector2f pos);
	~Obstacle();

	void update(float dt);

protected:
	sf::Vector2f position;
};

