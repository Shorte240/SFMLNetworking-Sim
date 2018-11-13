#include "Sim.h"

Sim::Sim(sf::RenderWindow* hwnd, Input* in)
{
	window = hwnd;
	input = in;

	// Initialise boid positions
	// boidManager.initialisePositions();
}

Sim::~Sim()
{
}

void Sim::handleInput(float dt)
{
}

void Sim::update(float dt)
{
	boid.update(dt);

	// BOIDS PROGRAM PSEUDOCODE
	
	// LOOP
	// draw_boids()
	// move_all_boids_to_new_positions()
	// END LOOP
}

void Sim::render()
{
	beginDraw();

	window->draw(boid);

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
