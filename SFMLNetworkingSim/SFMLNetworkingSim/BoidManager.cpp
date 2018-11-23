#include "BoidManager.h"

BoidManager::BoidManager(sf::RenderWindow* hwnd, Input* in)
{
	window = hwnd;
	input = in;

	for (int i = 0; i < NUM_BOIDS; i++)
	{
		Boids.push_back(Boid());
	}

	// Initialise boid positions
	initialisePositions();

	// Initial fps value
	fps = 0.0f;

	// Set the speed
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

void BoidManager::update(float dt)
{
	fps = 1.0f / dt;

	moveBoids(dt);

	outputText();

	if (input->isMouseLeftDown())
	{
		input->setMouseLeftDown(false);
		Obstacles.push_back(Obstacle(sf::Vector2f(input->getMouseX(),input->getMouseY())));
	}

	for (auto& b : Boids)
	{
		std::cout << "Boid X: " << b.getPosition().x << ", Boid Y: " << b.getPosition().y << std::endl;
	}
}

void BoidManager::render(sf::RenderWindow * window)
{
	// Draw all the boids
	for (auto& b : Boids)
	{
		window->draw(b);
	}

	// Draw the obstacle
	for (auto& o : Obstacles)
	{
		window->draw(o);
	}

	// Render the text
	window->draw(fpsText);
	window->draw(boidSeparationText);
}

// Move all the boids according to the rules.
void BoidManager::moveBoids(float dt)
{
	sf::Vector2f v1, v2, v3, v4, v5, v6;

	for (auto& b : Boids)
	{
		v1 = moveTowardsGroup(b, dt);
		v2 = distanceCheck(b, dt);
		v3 = matchVelocity(b, dt);
		v4 = seekPlace(b, dt, sf::Vector2f(input->getMouseX(), input->getMouseY()));
		v5 = boundPositions(b, dt);
		v6 = avoidPlace(b, dt);

		b.setBoidVelocity(b.getBoidVelocity() + v1 + v2 + v3 + v4 + v5 + v6);
		b.move(b.getBoidVelocity() * speed * dt);
		limitVelocity(b, dt);
		float angle = (atan2(b.getBoidVelocity().x, -b.getBoidVelocity().y) * 180 / 3.1415);
		b.setRotation(angle);
	}
}

// Output all crucial variables to the screen via text
void BoidManager::outputText()
{
	// FPS text
	fpsText.setFont(font);
	fpsText.setCharacterSize(12);
	fpsText.setString("FPS: " + std::to_string(fps));
	fpsText.setFillColor(sf::Color::White);
	fpsText.setPosition(window->getSize().x - 162, 0);

	// Boid separation value text
	boidSeparationText.setFont(font);
	boidSeparationText.setCharacterSize(12);
	boidSeparationText.setString("Separation Value: " + std::to_string(separationValue));
	boidSeparationText.setFillColor(sf::Color::White);
	boidSeparationText.setPosition(window->getSize().x - 162, 12);
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
sf::Vector2f BoidManager::moveTowardsGroup(Boid& bj, float dt)
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

	// Move the boids 1% of the way towards the centre of mass
	return (((perceivedCentre - bj.getPosition()) / 100.0f) * dt);
}

// Rule 2: Boids try to keep a small distance away from other objects (including other boids).
sf::Vector2f BoidManager::distanceCheck(Boid& bj, float dt)
{
	sf::Vector2f currentDistance = sf::Vector2f(0.0f, 0.0f);

	if (input->isKeyDown(sf::Keyboard::A))
	{
		input->setKeyUp(sf::Keyboard::A);
		if (separationValue > 1)
		{
			separationValue -= 1;
		}
	}
	else if (input->isKeyDown(sf::Keyboard::Q))
	{
		input->setKeyUp(sf::Keyboard::Q);
		if (separationValue < 100)
		{
			separationValue += 1;
		}
	}
	else
	{
		separationValue = separationValue;
	}

	for (auto& b : Boids)
	{
		if (b.getPosition() != bj.getPosition())
		{
			if (abs(b.getPosition().x - bj.getPosition().x) < separationValue)
			{
				if (abs(b.getPosition().y - bj.getPosition().y) < separationValue)
				{
					currentDistance = currentDistance - (b.getPosition() - bj.getPosition());
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
sf::Vector2f BoidManager::seekPlace(Boid & bj, float dt, sf::Vector2f place)
{
	return (((place - bj.getPosition()) / 100.0f) * dt);
}

// Rule 5: Limiting the speed of the boids.
void BoidManager::limitVelocity(Boid & bj, float dt)
{
	float speedLimit;
	speedLimit = 10.0f;
	sf::Vector2f newVelocity;

	if (abs(bj.getBoidVelocity().x) > speedLimit)
	{
		if (abs(bj.getBoidVelocity().y) > speedLimit)
		{
			newVelocity = ((bj.getBoidVelocity()) / ((abs(bj.getBoidVelocity().x)) * (abs(bj.getBoidVelocity().y)))) * speedLimit;
			bj.setBoidVelocity(newVelocity);
		}
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
	else if (bj.getPosition().x > yMax)
	{
		v.y = -10.0f;
	}

	return (v * dt);
}

// Rule 7: Tendency away from a particular place.
sf::Vector2f BoidManager::avoidPlace(Boid & bj, float dt)
{
	sf::Vector2f v;
	float m = 100.0f;
	sf::Vector2f place;
	float dist = 50.0f;

	for (auto& o : Obstacles)
	{
		if (abs(bj.getPosition().x - o.getPosition().x) < dist)
		{
			if (abs(bj.getPosition().y - o.getPosition().y) < dist)
			{
				place.x = o.getPosition().x;
				place.y = o.getPosition().y;
				v = -m * seekPlace(bj, dt, place);
			}
		}
	}

	return v;
}
