// Server class for the SFML server

#pragma once

#include "Includes.h"
#include "BoidManager.h"
#include "ObstacleManager.h"

// The IP address for the server
#define SERVERIP "127.0.0.1"

// The UDP port number for the server
#define SERVERPORT 4444

// The (fixed) size of message that we send between the two programs
// --- MAY NOT BE NEEDED --- //
#define MESSAGESIZE 40

class Server
{
public:
	Server(sf::RenderWindow* hwnd, Input *input);
	~Server();

	// Functions
	void update(float dt);
	void render(sf::RenderWindow* window);

protected:
	// Server set-up, TCP or UDP
	void tcpServer();
	void udpServer();

	// Communication with client, TCP or UDP
	void talk_to_client_tcp(sf::TcpSocket& clientSocket);
	void talk_to_client_udp(sf::UdpSocket& clientSocket);

	// NEED TO FIND BETTER WAY TO HANDLE ERRORS
	void die(const char *message);

private:
	// Background setup
	sf::RenderWindow* window;
	Input* input;

	// Entity Setup
	BoidManager* serverBoidManager;
	ObstacleManager* serverObstacleManager;

	std::list<BoidManager*> allBoidManagers;
	std::list<ObstacleManager*> allObstacleManagers;
};

