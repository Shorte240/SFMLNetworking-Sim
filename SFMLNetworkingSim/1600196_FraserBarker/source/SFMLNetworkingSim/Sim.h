// State to manage the simulation.

#pragma once

#include "Includes.h"
#include "Server.h"
#include "Client.h"

class Sim
{
public:
	Sim(sf::RenderWindow* hwnd, Input *input);
	~Sim();

	// Functions
	void update(float dt);
	void render();
	void reset();

private:
	// Background setup
	sf::RenderWindow* window;
	Input* input;

	// Functions
	void updateText();

	// Text setup
	sf::Font font;
	int fps;
	sf::Text fpsText;

	// Network setup
	Server* server;
	Client* client;

	// Draw functions
	void beginDraw();
	void endDraw();

	// Vars
	bool isServer;
	bool isClient;
};

