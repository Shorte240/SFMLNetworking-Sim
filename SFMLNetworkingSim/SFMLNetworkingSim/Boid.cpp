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

void Boid::predictPosition(float time)
{
	const int msize = messages_.size();
	assert(msize >= 3);
	const BoidData& msg0 = messages_[msize - 1];
	const BoidData& msg1 = messages_[msize - 2];
	const BoidData& msg2 = messages_[msize - 3];

	// No Model (just using latest data)
	/*position.x = msg0.positionX;
	position.y = msg0.positionY;*/

	// Linear
	float msgTime = msg0.time - msg1.time;
	float msgSpeedX = (msg0.positionX - msg1.positionX) / msgTime;
	float msgSpeedY = (msg0.positionY - msg1.positionY) / msgTime;

	float timeDiff = time - msg0.time;

	position.x = msg0.positionX + (msgSpeedX * timeDiff);
	position.y = msg0.positionY + (msgSpeedY * timeDiff);

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

void Boid::addMessage(const BoidData & msg)
{
	messages_.push_back(msg);
}

void Boid::setBoidVelocity(sf::Vector2f vel)
{ 
	velocity = vel; 
}
