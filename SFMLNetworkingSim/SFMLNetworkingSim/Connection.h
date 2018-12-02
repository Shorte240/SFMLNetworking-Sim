// Connection class
// Stores basic information about connections

#pragma once

#include "Includes.h"
#include "Protocols.h"

class Connection
{
public:
	Connection();
	~Connection();

	// Boids and Obstacles
	std::vector<BoidManager> connectionBoidManager;
	std::vector<ObstacleManager> connectionObstacleManager;

	// Server vars
	int ID;
	int boidCount;
	sf::IpAddress connectionAddress;
	unsigned short connectionPort;
};

