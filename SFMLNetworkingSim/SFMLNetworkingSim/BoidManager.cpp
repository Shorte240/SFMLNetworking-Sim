#include "BoidManager.h"

BoidManager::BoidManager(sf::RenderWindow* hwnd)
{
	window = hwnd;

	for (int i = 0; i < NUM_BOIDS; i++)
	{
		Boids.push_back(Boid());
	}

	// Initialise boid positions
	initialisePositions();
}

BoidManager::~BoidManager()
{
}

void BoidManager::update(float dt)
{
	// BOIDS PROGRAM PSEUDOCODE

	/*for (auto& b : Boids)
	{
		b.update(dt);
	}*/

	// LOOP
	moveBoids(dt);
	// END LOOP
}

void BoidManager::render(sf::RenderWindow * window)
{
	for (auto& b : Boids)
	{
		window->draw(b);
	}
}

// Move all the boids according to the rules.
void BoidManager::moveBoids(float dt)
{
	sf::Vector2f v1, v2, v3;

	for (auto& b : Boids)
	{
		v1 = rule1(b);
		v2 = rule2(b);
		v3 = rule3(b);

		b.setBoidVelocity(b.getBoidVelocity() + v1 + v2 + v3);
		b.setBoidPosition(b.getBoidPosition() + b.getBoidVelocity());
	}
}

// Give initial positions to each boid.
void BoidManager::initialisePositions()
{
	srand(time(NULL));
	for (auto& b : Boids)
	{
		float randX = (rand() % window->getSize().x);
		float randY = (rand() % window->getSize().y);;
		sf::Vector2f newPos = sf::Vector2f(randX, randY);
		b.setBoidPosition(newPos);
	}
}

// Rule 1: Boids try to fly towards the centre of mass of neighbouring boids.
sf::Vector2f BoidManager::rule1(Boid& bj)
{
	// Move the boid towards the perceivedCentre
	sf::Vector2f perceivedCentre;

	for (auto& b: Boids)
	{
		if (b.getBoidPosition() != bj.getBoidPosition())
		{
			perceivedCentre = perceivedCentre + b.getBoidPosition();
		}
	}

	perceivedCentre = (perceivedCentre / (NUM_BOIDS - 1.0f));

	return ((perceivedCentre - bj.getBoidPosition()) / 100.0f);
}

// Rule 2: Boids try to keep a small distance away from other objects (including other boids).
sf::Vector2f BoidManager::rule2(Boid& bj)
{
	sf::Vector2f currentDistance = sf::Vector2f(0.0f, 0.0f);

	for (auto& b : Boids)
	{
		if (b.getBoidPosition() != bj.getBoidPosition())
		{
			if (abs(b.getBoidPosition().x - bj.getBoidPosition().x) < 100.0f)
			{
				if (abs(b.getBoidPosition().y - bj.getBoidPosition().y) < 100.0f)
				{
					currentDistance = currentDistance - (b.getBoidPosition() - bj.getBoidPosition()); 
				}
			}
		}
	}

	return currentDistance;
}

// Rule 3: Boids try to match velocity with near boids.
sf::Vector2f BoidManager::rule3(Boid& bj)
{
	sf::Vector2f perceivedVelocity;

	for (auto& b : Boids)
	{
		if (b.getBoidPosition() != bj.getBoidPosition())
		{
			perceivedVelocity = perceivedVelocity + b.getBoidVelocity();
		}
	}

	perceivedVelocity = (perceivedVelocity / (NUM_BOIDS - 1.0f));

	return ((perceivedVelocity - bj.getBoidVelocity()) / 8.0f);
}
