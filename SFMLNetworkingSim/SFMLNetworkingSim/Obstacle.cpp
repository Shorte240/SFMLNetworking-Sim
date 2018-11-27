#include "Obstacle.h"

Obstacle::Obstacle(sf::Vector2f pos) : CircleShape()
{
	// Set shape of boid
	setRadius(10.0f);

	// Initialise position to 0.0f.
	position.x = pos.x - getRadius();
	position.y = pos.y - getRadius();

	setPosition(position);
	setFillColor(sf::Color::Red);
}

Obstacle::~Obstacle()
{
}

void Obstacle::update(float dt)
{
}
