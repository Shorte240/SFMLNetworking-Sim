#include "Sim.h"

Sim::Sim(sf::RenderWindow* hwnd, Input* in)
{
	window = hwnd;
	input = in;

	// Initial fps value
	fps = 0.0f;

	// Initialise new boid manager
	boidManager = new BoidManager(window, input);

	// Initialise new obstacle manager
	obstacleManager = new ObstacleManager(window, input);

	// Set up font
	if (!font.loadFromFile("font/arial.ttf"))
	{
		std::cout << "Font can't load" << std::endl;
	}
}

Sim::~Sim()
{
}

void Sim::handleInput(float dt)
{
}

void Sim::update(float dt)
{
	// Calculate FPS
	fps = 1.0f / dt;

	// Update obstacle manager
	obstacleManager->update(dt);

	// Update boid manager
	boidManager->update(dt, obstacleManager->getObstacles());

	// Update all the text variables
	updateText();
}

void Sim::render()
{
	beginDraw();

	// Draw obstacles in obstacle manager
	obstacleManager->render(window);

	// Draw boids in boid manager
	boidManager->render(window);

	// Draw FPS text
	window->draw(fpsText);

	endDraw();
}

void Sim::reset()
{
}

void Sim::updateText()
{
	// FPS text
	fpsText.setFont(font);
	fpsText.setCharacterSize(12);
	fpsText.setString("FPS: " + std::to_string(fps));
	fpsText.setFillColor(sf::Color::White);
	fpsText.setPosition(window->getSize().x - 162, 0);
}

void Sim::beginDraw()
{
	window->clear(sf::Color::Black);
}

void Sim::endDraw()
{
	window->display();
}
