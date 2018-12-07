// Boid class
// Basic movement and setup

#pragma once

#include "Includes.h"
#include <cassert>
#include "Protocols.h"

class Boid : public sf::CircleShape
{
public:
	Boid(int id, sf::Vector2f& pos, sf::Vector2f& vel, sf::Color& col);
	~Boid();

	void update(float dt);

	void predictPosition(float time);

	void addMessage(const BoidData& msg);

	// Get the Boids velocity
	sf::Vector2f getBoidVelocity() { return velocity; };
	int getBoidID() { return ID; };

	// Set the Boids velocity
	void setBoidVelocity(sf::Vector2f vel);
	void setBoidID(int id) { ID = id; };

protected:
	int ID;
	sf::Vector2f velocity;
	sf::Vector2f position;

	std::vector<BoidData> messages_;
};

