// Boid.cpp
// Sets the boids ID, position, velocity, radius, fill colour and point count

#include "Boid.h"

Boid::Boid(int id, sf::Vector2f& pos, sf::Vector2f& vel, sf::Color& col) : CircleShape()
{
	// Set the boids ID
	ID = id;

	// Set the boids position
	position.x = pos.x;
	position.y = pos.y;

	// Set the boids velocity
	velocity.x = vel.x;
	velocity.y = vel.y;

	// Set boid radius
	setRadius(5.0f);
	// Set boid point count (3 for a triangle)
	setPointCount(3);

	// Set the position
	setPosition(pos);
	
	// Set the fill colour
	setFillColor(col);
}

Boid::~Boid()
{
}

void Boid::update(float dt)
{
	// Continually update the boids position variable
	position = getPosition();
}

void Boid::predictPosition(float time)
{
	// Get the size of the messages
	const int msize = messages_.size();

	if (msize >= 3 && ID != -1)
	{
		// Get three messages from the boids message history
		const BoidData& msg0 = messages_[msize - 1];
		const BoidData& msg1 = messages_[msize - 2];
		const BoidData& msg2 = messages_[msize - 3];

		// No Model (just using latest data)
		/*position.x = msg0.positionX;
		position.y = msg0.positionY;*/

		// Linear
		// Calculate the time difference between the messages
		float msgTime = msg0.time - msg1.time;
		// Calculate the x speed difference
		float msgSpeedX = (msg0.positionX - msg1.positionX) / msgTime;
		// Calculate the y speed difference
		float msgSpeedY = (msg0.positionY - msg1.positionY) / msgTime;
		// Calculate the difference between the time of the simulation
		// and the most recent message time
		float timeDiff = time - msg0.time;

		// Set the boids position to the predicted position
		position.x = msg0.positionX + (msgSpeedX * timeDiff);
		position.y = msg0.positionY + (msgSpeedY * timeDiff);

		// Set the position of the boid
		setPosition(position);

		// Quadratic
		// v^2 = u^2 + 2as
		/*float msgTime = msg0.time - msg1.time;
		float msgSpeedX = (msg0.positionX - msg1.positionX) / msgTime;
		float msgSpeedY = (msg0.positionY - msg1.positionY) / msgTime;
		float acc = (msg2.positionX - msg1.positionX) / (msg2.time - msg1.time);

		float timeDiff = time - msg0.time;

		position.x = msg0.positionX + (msgSpeedX * timeDiff);
		position.y = msg0.positionY + (msgSpeedY * timeDiff);*/
	}
}

void Boid::addMessage(const BoidData & msg)
{
	// Add a message to the boids message history
	messages_.push_back(msg);
}

void Boid::setBoidVelocity(sf::Vector2f vel)
{ 
	// Set the boids velocity
	velocity = vel; 
}
