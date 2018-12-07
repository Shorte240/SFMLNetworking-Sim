// Server class for the SFML server

#pragma once

#include "Includes.h"
#include "Connection.h"

// The IP address for the server
#define SERVERIP "127.0.0.1"

// The UDP port number for the server
#define SERVERPORT 4444
#define SERVERPORT2 4445

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
	void udpServerSocketSetup();

	// Communication with client, TCP or UDP
	void talk_to_client_tcp(sf::TcpSocket& clientSocket);
	void receiveBoidInfo(sf::UdpSocket& clientSocket);
	void receiveObstacleInfo(sf::UdpSocket& clientSocket);

	// NEED TO FIND BETTER WAY TO HANDLE ERRORS
	void die(const char *message);

private:
	// Background setup
	sf::RenderWindow* window;
	Input* input;
	float totalTime;
	float tickTimer;
	std::vector<ObstacleData> obsMsgs;

	// Server setup
	sf::UdpSocket boidSocket;
	sf::UdpSocket obstacleSocket;
	std::vector<Connection*> connections;

	// Entity Setup
	BoidManager* serverBoidManager;
	ObstacleManager* serverObstacleManager;
};

