#include "Boid.h"

Boid::Boid(const sf::Vector2f & size) : CircleShape()
{
	// Initialise position and velocity of Boid to 0.0f.
	position.x = 0.0f;
	position.y = 0.0f;

	velocity.x = 0.0f;
	velocity.x = 0.0f;

	// Set shape of boid
	setRadius(5.0f);
	setPointCount(3);

	setPosition(position);
	float r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 255.0f));
	float g = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 255.0f));
	float b = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 255.0f));
	
	setFillColor(sf::Color(r, g, b, 255.0f));
}

Boid::~Boid()
{
}

void Boid::update(float dt)
{
}

void Boid::setBoidVelocity(sf::Vector2f vel)
{ 
	velocity = vel; 
}
