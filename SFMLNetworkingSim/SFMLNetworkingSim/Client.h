// Client class for the SFML server

#pragma once

#include "Includes.h"
#include "Connection.h"
#include "BoidManager.h"
#include "ObstacleManager.h"

// The IP address of the server to connect to
#define SERVERIP "127.0.0.1"

// The port number on the server to connect to
#define SERVERPORT 4444
#define SERVERPORT2 4445

// The (fixed) size of message that we send between the two programs
#define MESSAGESIZE 40

class Client
{
public:
	Client(sf::RenderWindow* hwnd, Input *input);
	~Client();

	// Update the client
	void update(float dt);
	// Render the clients simulation
	void render(sf::RenderWindow* window);

protected:
	// Bind the port and set the socket to non blocking
	void udpClientSocketSetup();
	// Attempt a connection with UDP to the server
	void connectToUDPServer(sf::UdpSocket& socket);
	// Send/Receive information about boids
	void receiveBoidInfo(sf::UdpSocket& socket);
	// Send/Receive information about obstacles
	void receiveObstacleInfo(sf::UdpSocket& clientSocket);
	// Error to give if there was a problem with send/receive
	void die(const char *message);

private:
	// SFML setup
	sf::RenderWindow* window;
	Input* input;

	// Sockets
	sf::UdpSocket boidSocket;
	sf::UdpSocket obstacleSocket;

	// Entity Setup
	BoidManager* clientBoidManager;
	ObstacleManager* clientObstacleManager;

	// Variables
	// Total time the client has been running
	float totalTime;
	// Tick count as to when messages should be sent/received
	float tickTimer;
	// Clients ID
	int clientID;
	// A message history about boids
	std::vector<BoidData> boidMsgs;
	// A message history about obstacles
	std::vector<ObstacleData> obsMsgs;
};

