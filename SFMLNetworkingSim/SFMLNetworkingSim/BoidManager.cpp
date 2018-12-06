#include "BoidManager.h"

BoidManager::BoidManager(sf::RenderWindow* hwnd, Input* in)
{
	window = hwnd;
	input = in;

	// Initialise boid positions and colours
	initialisePositions();

	// Set the boid speed
	speed = 10.f;

	// Set the distance boids are spaced apart
	separationValue = 50;

	// Set up font
	if (!font.loadFromFile("font/arial.ttf"))
	{
		std::cout << "Font can't load" << std::endl;
	}
}

BoidManager::~BoidManager()
{
}

void BoidManager::update(float dt, std::vector<Obstacle>& obs)
{
	moveBoids(dt, obs);

	/*for (auto& boid : boidFlock)
	{
		std::cout << "Boid X: " << boid.getPosition().x << ", Boid Y: " << boid.getPosition().y << std::endl;
	}*/

	updateText();
}

void BoidManager::render(sf::RenderWindow * window)
{
	// Draw all the boids
	for (auto& boid : boidFlock)
	{
		window->draw(boid);
	}

	// Render the text
	window->draw(boidSeparationText);
	window->draw(boidCountText);
}

void BoidManager::addBoidToFlock(int ID, float posX, float posY, float velX, float velY, int r, int g, int b, int a)
{
	sf::Vector2f newPos = sf::Vector2f(posX, posY);
	sf::Vector2f newVel = sf::Vector2f(velX, velY);
	sf::Color col = sf::Color(r, g, b, a);
	boidFlock.push_back(Boid(ID, newPos, newVel, col));
}

// Move all the boids according to the rules.
void BoidManager::moveBoids(float dt, std::vector<Obstacle>& obs)
{
	sf::Vector2f v1, v2, v3,/*v4, /*v5,*/ v6;

	for (auto& boid : boidFlock)
	{
		v1 = moveTowardsGroup(boid, dt);
		v2 = distanceCheck(boid, dt);
		v3 = matchVelocity(boid, dt);
		//v4 = seekPlace(boid, dt, sf::Vector2f(input->getMouseX(), input->getMouseY()));
		//v5 = boundPositions(boid, dt);
		v6 = avoidPlace(boid, dt, obs);

		boid.setBoidVelocity(boid.getBoidVelocity() + v1 + v2 + v3 + /*v4 + v5 +*/ v6);
		boid.move(boid.getBoidVelocity() * speed * dt);
		limitVelocity(boid, dt);
		positionWindowWrapping(boid, dt);
		float angle = (atan2(boid.getBoidVelocity().x, -boid.getBoidVelocity().y) * 180 / 3.1415);
		boid.setRotation(angle);
	}
}

// Update all text variables
void BoidManager::updateText()
{
	// Boid separation value text
	boidSeparationText.setFont(font);
	boidSeparationText.setCharacterSize(12);
	boidSeparationText.setString("Separation Value: " + std::to_string(separationValue));
	boidSeparationText.setFillColor(sf::Color::White);
	boidSeparationText.setPosition(window->getSize().x - 162, 12);

	// Boid separation value text
	boidCountText.setFont(font);
	boidCountText.setCharacterSize(12);
	boidCountText.setString("Boid Count: " + std::to_string(boidFlock.size()));
	boidCountText.setFillColor(sf::Color::White);
	boidCountText.setPosition(window->getSize().x - 162, 24);
}

// Give initial positions to each boid.
void BoidManager::initialisePositions()
{
	// Initialise random number generator
	srand(time(NULL));

	// Calculate random float from 0 - 255 for colour values of boids
	float r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 255.0f));
	float g = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 255.0f));
	float b = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 255.0f));
	sf::Color col(r, g, b, 255.0f);

	for (int i = 0; i < NUM_BOIDS; i++)
	{
		// Caclulate random X & Y positions for each boid
		float randX = (rand() % window->getSize().x);
		float randY = (rand() % window->getSize().y);
		sf::Vector2f newPos = sf::Vector2f(randX, randY);
		sf::Vector2f newVel = sf::Vector2f(0.0f, 0.0f);

		// Add a new boid based on the position and colour generated
		boidFlock.push_back(Boid(-1, newPos, newVel, col));
	}
}

// Rule 1: Boids try to fly towards the centre of mass of neighbouring boids.
sf::Vector2f BoidManager::moveTowardsGroup(Boid& bj, float dt)
{
	// Move the boid towards the perceivedCentre
	sf::Vector2f perceivedCentre;

	for (auto& boid: boidFlock)
	{
		if (boid.getPosition() != bj.getPosition())
		{
			perceivedCentre = perceivedCentre + boid.getPosition();
		}
	}

	perceivedCentre = (perceivedCentre / (NUM_BOIDS - 1.0f));

	// Move the boids 1% of the way towards the centre of mass
	return (((perceivedCentre - bj.getPosition()) / 100.0f) * dt);
}

// Rule 2: Boids try to keep a small distance away from other objects (including other boids).
sf::Vector2f BoidManager::distanceCheck(Boid& bj, float dt)
{
	sf::Vector2f currentDistance = sf::Vector2f(0.0f, 0.0f);

	if (input->isKeyDown(sf::Keyboard::S))
	{
		input->setKeyUp(sf::Keyboard::S);
		if (separationValue > 1)
		{
			separationValue -= 1;
		}
	}
	else if (input->isKeyDown(sf::Keyboard::W))
	{
		input->setKeyUp(sf::Keyboard::W);
		if (separationValue < 100)
		{
			separationValue += 1;
		}
	}
	else
	{
		separationValue = separationValue;
	}

	for (auto& boid : boidFlock)
	{
		if (boid.getPosition() != bj.getPosition())
		{
			if (abs(boid.getPosition().x - bj.getPosition().x) < separationValue)
			{
				if (abs(boid.getPosition().y - bj.getPosition().y) < separationValue)
				{
					currentDistance = currentDistance - (boid.getPosition() - bj.getPosition());
				}
			}
		}
	}

	return (currentDistance * dt);
}

// Rule 3: Boids try to match velocity with near boids.
sf::Vector2f BoidManager::matchVelocity(Boid& bj, float dt)
{
	sf::Vector2f perceivedVelocity;

	for (auto& boid : boidFlock)
	{
		if (boid.getPosition() != bj.getPosition())
		{
			perceivedVelocity = perceivedVelocity + boid.getBoidVelocity();
		}
	}

	perceivedVelocity = (perceivedVelocity / (NUM_BOIDS - 1.0f));

	return (((perceivedVelocity - bj.getBoidVelocity()) / 8.0f) * dt);
}

// Rule 4: Tendency towards a particular place
sf::Vector2f BoidManager::seekPlace(Boid & bj, float dt, sf::Vector2f place)
{
	return (((place - bj.getPosition()) / 100.0f) * dt);
}

// Rule 5: Limiting the speed of the boids.
void BoidManager::limitVelocity(Boid & bj, float dt)
{
	float speedLimit;
	speedLimit = 30.0f;
	sf::Vector2f newVelocity;

	if (abs(bj.getBoidVelocity().x) > speedLimit)
	{
		newVelocity = ((bj.getBoidVelocity()) / ((abs(bj.getBoidVelocity().x)))) * speedLimit;
		bj.setBoidVelocity(newVelocity);
	}
	else if (abs(bj.getBoidVelocity().y) > speedLimit)
	{
		newVelocity = ((bj.getBoidVelocity()) / ((abs(bj.getBoidVelocity().y)))) * speedLimit;
		bj.setBoidVelocity(newVelocity);
	}
	else
	{
		bj.setBoidVelocity(bj.getBoidVelocity());
	}
}

// Check if the boids are outwith the window and alter their position accordingly
void BoidManager::positionWindowWrapping(Boid & bj, float dt)
{
	float xMin, xMax, yMin, yMax;
	xMin = 0.0f;
	xMax = window->getSize().x;
	yMin = 0.0f;
	yMax = window->getSize().y;

	if (bj.getPosition().x < xMin)
	{
		bj.setPosition(bj.getPosition().x + xMax, bj.getPosition().y);
	}
	else if (bj.getPosition().x > xMax)
	{
		bj.setPosition(bj.getPosition().x - xMax, bj.getPosition().y);
	}
	if (bj.getPosition().y < yMin)
	{
		bj.setPosition(bj.getPosition().x, bj.getPosition().y + yMax);
	}
	else if (bj.getPosition().y > yMax)
	{
		bj.setPosition(bj.getPosition().x, bj.getPosition().y - yMax);
	}
}

// Rule 6: Bound the positions of the boids within the window.
sf::Vector2f BoidManager::boundPositions(Boid & bj, float dt)
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
	else if (bj.getPosition().y > yMax)
	{
		v.y = -10.0f;
	}

	return (v * dt);
}

// Rule 7: Tendency away from a particular place.
sf::Vector2f BoidManager::avoidPlace(Boid & bj, float dt, std::vector<Obstacle>& obs)
{
	sf::Vector2f v;
	float m = 100.0f;
	sf::Vector2f place;
	float dist = 50.0f;

	for (auto& obstacle : obs)
	{
		if (abs(bj.getPosition().x - obstacle.getPosition().x) < dist)
		{
			if (abs(bj.getPosition().y - obstacle.getPosition().y) < dist)
			{
				place.x = obstacle.getPosition().x;
				place.y = obstacle.getPosition().y;
				v = -m * seekPlace(bj, dt, place);
			}
		}
	}

	return v;
}
