#include "Obstacle.h"

Obstacle::Obstacle(int id, sf::Vector2f pos) : CircleShape()
{
	// Set ID
	ID = id;

	// Set shape of boid
	setRadius(10.0f);

	setOrigin(sf::Vector2f(getRadius(), getRadius()));

	// Initialise position to 0.0f.
	position.x = pos.x;
	position.y = pos.y;

	setPosition(position);
	setFillColor(sf::Color::Red);
}

Obstacle::~Obstacle()
{
}

void Obstacle::update(float dt)
{
}
