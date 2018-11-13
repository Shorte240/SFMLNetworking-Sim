#include "Boid.h"

Boid::Boid(const sf::Vector2f & size) : CircleShape()
{
	// Initialise position and velocity of Boid to 0.0f.
	position.x = 0.0f;
	position.y = 0.0f;

	velocity.x = 0.0f;
	velocity.x = 0.0f;

	// Set shape of boid
	setRadius(80.0f);
	setPointCount(3);
	setPosition(10.f, 10.f);
	setFillColor(sf::Color::Yellow);
}

Boid::~Boid()
{
}

void Boid::update(float dt)
{
}
