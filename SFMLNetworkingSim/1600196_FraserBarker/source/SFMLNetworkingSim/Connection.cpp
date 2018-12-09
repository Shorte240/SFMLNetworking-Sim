#include "Connection.h"

Connection::Connection(sf::RenderWindow* hwnd, Input* in)
{
	// Initialise new boid manager
	connectionBoidManager = new BoidManager(hwnd, in);

	// As the boid will be added from a connecting client,
	// Erase the default current generated ones for this class.
	connectionBoidManager->getBoidFlock().clear();

	// Initialise new obstacle manager
	connectionObstacleManager = new ObstacleManager(hwnd, in);
}

Connection::~Connection()
{
}
