// Client class for the SFML server

#pragma once

#include "Includes.h"
#include "Connection.h"
#include "BoidManager.h"
#include "ObstacleManager.h"

// The IP address of the server to connect to
#define SERVERIP "127.0.0.1"

// The TCP port number on the server to connect to
#define SERVERPORT 4444
#define SERVERPORT2 4445

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
	void receiveBoidInfo(sf::UdpSocket& socket);
	void receiveObstacleInfo(sf::UdpSocket& clientSocket);
	void die(const char *message);

private:
	// Background setup
	sf::RenderWindow* window;
	Input* input;

	// Client setup
	sf::UdpSocket boidSocket;
	sf::UdpSocket obstacleSocket;

	// Entity Setup
	BoidManager* clientBoidManager;
	ObstacleManager* clientObstacleManager;

	// Vars
	float totalTime;
	float tickTimer;
	int clientID;
	bool sentBoids = false;
	bool gotID = false;
	std::vector<BoidData> boidMsgs;
	std::vector<ObstacleData> obsMsgs;
};

