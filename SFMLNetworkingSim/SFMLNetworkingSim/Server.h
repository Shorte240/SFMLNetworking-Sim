// Server class for the SFML server

#pragma once

#include "Includes.h"
#include "Connection.h"
#include "BoidManager.h"
#include "ObstacleManager.h"

// The IP address for the server
#define SERVERIP "127.0.0.1"

// The UDP port number for the server
#define SERVERPORT 4444
#define SERVERPORT2 4445

class Server
{
public:
	Server(sf::RenderWindow* hwnd, Input *input);
	~Server();

	// Update the server
	void update(float dt);
	// Render the servers simulation
	void render(sf::RenderWindow* window);

protected:
	// Server socket set-up for UDP
	void udpServerSocketSetup();

	// Communication with client about boids via UDP
	void receiveBoidInfo(sf::UdpSocket& clientSocket);
	// Communication with client about obstacles via UDP
	void receiveObstacleInfo(sf::UdpSocket& clientSocket);

	// Error to give if cant send/receive
	void die(const char *message);

private:
	// SFML setup
	sf::RenderWindow* window;
	Input* input;

	// Total time the server has been running
	float totalTime;
	// Tick count
	float tickTimer;
	// Message history about boids
	std::vector<BoidData> boidMsgs;
	// Message history about obstacles
	std::vector<ObstacleData> obsMsgs;

	// Server setup
	sf::UdpSocket boidSocket;
	sf::UdpSocket obstacleSocket;
	std::vector<Connection*> connections;

	// Entity Setup
	BoidManager* serverBoidManager;
	ObstacleManager* serverObstacleManager;
};

