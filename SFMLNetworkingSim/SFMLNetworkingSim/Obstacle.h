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

	// Get the obstacles ID
	int getID() { return ID; };

	// Set the obstacles ID
	void setID(int id) { ID = id; };

protected:
	// Position of the obstacle
	sf::Vector2f position;

	// ID of the obstacle
	int ID;
};

