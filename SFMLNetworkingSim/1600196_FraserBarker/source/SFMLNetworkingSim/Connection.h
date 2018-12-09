// Connection class
// Stores basic information about connections

#pragma once

#include "Includes.h"
#include "Protocols.h"
#include "BoidManager.h"
#include "ObstacleManager.h"

class Connection
{
public:
	Connection(sf::RenderWindow* hwnd, Input *input);
	~Connection();

	// Boids and Obstacles
	BoidManager* connectionBoidManager;
	ObstacleManager* connectionObstacleManager;

	// Server vars
	int ID;
	int boidCount;
	sf::IpAddress connectionAddress;
	unsigned short connectionPort;
};

