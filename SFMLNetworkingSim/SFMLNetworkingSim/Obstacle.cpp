#include "Obstacle.h"

Obstacle::Obstacle(const sf::Vector2f & size) : CircleShape()
{
	// Set shape of boid
	setRadius(10.0f);

	// Initialise position to 0.0f.
	position.x = size.x - getRadius();
	position.y = size.y - getRadius();

	setPosition(position);
	setFillColor(sf::Color::Red);
}

Obstacle::~Obstacle()
{
}

void Obstacle::update(float dt)
{
}
