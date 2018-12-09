// Boid.
// An object that is used to simulate a bird in a virtual simulation.
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

	// Update the boid
	void update(float dt);

	// Predict the boids position
	void predictPosition(float time);

	// Add a message to the boids message history
	void addMessage(const BoidData& msg);

	// Get the Boids velocity
	sf::Vector2f getBoidVelocity() { return velocity; };
	// Get the Boids ID
	int getBoidID() { return ID; };

	// Set the Boids velocity
	void setBoidVelocity(sf::Vector2f vel);
	// Set the Boids ID
	void setBoidID(int id) { ID = id; };

protected:
	// Boids ID
	int ID;
	// Boids velocity
	sf::Vector2f velocity;
	// Boids position
	sf::Vector2f position;
	// Boids message history
	std::vector<BoidData> messages_;
};

