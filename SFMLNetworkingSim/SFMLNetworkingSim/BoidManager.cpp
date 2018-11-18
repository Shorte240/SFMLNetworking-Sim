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
	moveBoids(dt);

	for (auto& b : Boids)
	{
		std::cout << "Boid X: " << b.getPosition().x << ", Boid Y: " << b.getPosition().y << std::endl;
	}
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
	sf::Vector2f v1, v2, v3, v4, v5, v6;

	for (auto& b : Boids)
	{
		v1 = rule1(b, dt);
		v2 = rule2(b, dt);
		v3 = rule3(b, dt);
		v4 = rule4(b, dt);
		v5 = rule5(b, dt);
		v6 = rule6(b, dt);

		b.setBoidVelocity(b.getBoidVelocity() + v1 + v2 + v3 + v4 + v5 + v6);
		b.move(b.getBoidVelocity() * dt);
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
		b.setPosition(newPos);
	}
}

// Rule 1: Boids try to fly towards the centre of mass of neighbouring boids.
sf::Vector2f BoidManager::rule1(Boid& bj, float dt)
{
	// Move the boid towards the perceivedCentre
	sf::Vector2f perceivedCentre;

	for (auto& b: Boids)
	{
		if (b.getPosition() != bj.getPosition())
		{
			perceivedCentre = perceivedCentre + b.getPosition();
		}
	}

	perceivedCentre = (perceivedCentre / (NUM_BOIDS - 1.0f));

	return (((perceivedCentre - bj.getPosition()) / 100.0f) * dt);
}

// Rule 2: Boids try to keep a small distance away from other objects (including other boids).
sf::Vector2f BoidManager::rule2(Boid& bj, float dt)
{
	sf::Vector2f currentDistance = sf::Vector2f(0.0f, 0.0f);

	for (auto& b : Boids)
	{
		if (b.getPosition() != bj.getPosition())
		{
			if (abs(b.getPosition().x - bj.getPosition().x) < 100.0f)
			{
				if (abs(b.getPosition().y - bj.getPosition().y) < 100.0f)
				{
					currentDistance = currentDistance - (b.getPosition() - bj.getPosition());
				}
			}
		}
	}

	return (currentDistance * dt);
}

// Rule 3: Boids try to match velocity with near boids.
sf::Vector2f BoidManager::rule3(Boid& bj, float dt)
{
	sf::Vector2f perceivedVelocity;

	for (auto& b : Boids)
	{
		if (b.getPosition() != bj.getPosition())
		{
			perceivedVelocity = perceivedVelocity + b.getBoidVelocity();
		}
	}

	perceivedVelocity = (perceivedVelocity / (NUM_BOIDS - 1.0f));

	return (((perceivedVelocity - bj.getBoidVelocity()) / 8.0f) * dt);
}

// Rule 4: Tendency towards a particular place
sf::Vector2f BoidManager::rule4(Boid & bj, float dt)
{
	sf::Vector2f place;

	place.x = window->getSize().x / 2.0f;
	place.y = window->getSize().y / 2.0f;

	return (((place - bj.getPosition()) / 100.0f) * dt);
}

// Rule 5: Limiting the speed of the boids.
sf::Vector2f BoidManager::rule5(Boid & bj, float dt)
{
	float speedLimit;
	speedLimit = 200.0f;
	sf::Vector2f v;

	if (abs(bj.getBoidVelocity().x) > speedLimit)
	{
		if (abs(bj.getBoidVelocity().y) > speedLimit)
		{
			sf::Vector2f newVelocity;
			newVelocity = ((bj.getBoidVelocity()) / (abs(bj.getBoidVelocity().x) * abs(bj.getBoidVelocity().y) * speedLimit));
			bj.setBoidVelocity(newVelocity * dt);
		}
	}
	return sf::Vector2f();
}

// Rule 6: Bound the positions of the boids within the window.
sf::Vector2f BoidManager::rule6(Boid & bj, float dt)
{
	float xMin, xMax, yMin, yMax;
	xMin = 0.0f;
	xMax = window->getSize().x;
	yMin = 0.0f;
	yMax = window->getSize().y;

	sf::Vector2f v;

	if (bj.getPosition().x < xMin)
	{
		v.x = 10.0f;
	}
	else if (bj.getPosition().x > xMax)
	{
		v.x = -10.0f;
	}
	if (bj.getPosition().y < yMin)
	{
		v.y = 10.0f;
	}
	else if (bj.getPosition().x > yMax)
	{
		v.y = -10.0f;
	}

	return (v * dt);
}
