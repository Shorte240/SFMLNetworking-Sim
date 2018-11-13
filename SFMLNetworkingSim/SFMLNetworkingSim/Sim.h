// State to manage the simulation.

#pragma once

#include "Includes.h"
#include "Server.h"
#include "Client.h"
#include "Boid.h"

class Sim
{
public:
	Sim(sf::RenderWindow* hwnd, Input *input);
	~Sim();

	// Functions
	void handleInput(float dt);
	void update(float dt);
	void render();
	void reset();

private:
	// Background setup
	sf::RenderWindow* window;
	sf::View* view;
	Input* input;

	// Text setup
	sf::Font font;

	// Server setup
	Server server;
	Client client;

	// Entity setup
	Boid boid;

	// Draw functions
	void beginDraw();
	void endDraw();
};

