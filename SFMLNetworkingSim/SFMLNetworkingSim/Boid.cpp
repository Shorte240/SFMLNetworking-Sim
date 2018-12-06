#include "Boid.h"

Boid::Boid(int id, sf::Vector2f& pos, sf::Vector2f& vel, sf::Color& col) : CircleShape()
{
	ID = id;

	// Init tracking position to 0.0f.
	position.x = pos.x;
	position.y = pos.y;

	// Initialise velocity of Boid to 0.0f.
	velocity.x = vel.x;
	velocity.y = vel.y;

	// Set shape of boid
	setRadius(5.0f);
	setPointCount(3);

	setPosition(pos);
	
	setFillColor(col);
}

Boid::~Boid()
{
}

void Boid::update(float dt)
{
	position = getPosition();
}

void Boid::setBoidVelocity(sf::Vector2f vel)
{ 
	velocity = vel; 
}
