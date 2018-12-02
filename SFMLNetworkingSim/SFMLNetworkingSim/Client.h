// Client class for the SFML server

#pragma once

#include "Includes.h"
#include "Connection.h"

// The IP address of the server to connect to
#define SERVERIP "127.0.0.1"

// The TCP port number on the server to connect to
#define SERVERPORT 4444

// The (fixed) size of message that we send between the two programs
#define MESSAGESIZE 40

class Client
{
public:
	Client(sf::RenderWindow* hwnd, Input *input);
	~Client();

	// Functions
	void update(float dt);
	void render(sf::RenderWindow* window);

protected:
	void tcpClient();
	void udpClientSocketSetup();
	void connectToUDPServer(sf::UdpSocket& socket);
	void talk_to_server_tcp(sf::TcpSocket& socket);
	void talk_to_server_udp(sf::UdpSocket& socket);
	void die(const char *message);

private:
	// Background setup
	sf::RenderWindow* window;
	Input* input;

	// Client setup
	sf::UdpSocket clientSocket;

	// Entity Setup
	BoidManager* clientBoidManager;
	ObstacleManager* clientObstacleManager;

	std::vector<BoidManager*> allBoidManagers;
	std::vector<ObstacleManager*> allObstacleManagers;

	// Vars
	float totalTime;
	int clientID;
};

