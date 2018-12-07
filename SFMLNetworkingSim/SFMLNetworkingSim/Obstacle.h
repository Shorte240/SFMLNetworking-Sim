// Obstacle class
// Boids will tend to avoid where this is placed

#pragma once

#include "Includes.h"

class Obstacle : public sf::CircleShape
{
public:
	Obstacle(int id, sf::Vector2f pos);
	~Obstacle();

	void update(float dt);

	// Getters
	int getID() { return ID; };

	// Setters
	void setID(int id) { ID = id; };

protected:
	sf::Vector2f position;
	int ID;
};

