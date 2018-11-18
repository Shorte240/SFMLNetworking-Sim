#include "Sim.h"

Sim::Sim(sf::RenderWindow* hwnd, Input* in)
{
	window = hwnd;
	input = in;

	boidManager = new BoidManager(window);
}

Sim::~Sim()
{
}

void Sim::handleInput(float dt)
{
}

void Sim::update(float dt)
{
	boidManager->update(dt);
}

void Sim::render()
{
	beginDraw();

	boidManager->render(window);

	endDraw();
}

void Sim::reset()
{
}

void Sim::beginDraw()
{
	window->clear(sf::Color::Black);
}

void Sim::endDraw()
{
	window->display();
}
