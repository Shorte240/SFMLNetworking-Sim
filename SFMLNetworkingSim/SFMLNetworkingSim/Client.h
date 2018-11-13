// Client class for the SFML server

#pragma once

#include "Includes.h"

// The IP address of the server to connect to
#define SERVERIP "127.0.0.1"

// The TCP port number on the server to connect to
#define SERVERPORT 4444

// The (fixed) size of message that we send between the two programs
#define MESSAGESIZE 40

class Client
{
public:
	Client();
	~Client();

	void tcpClient();
	void udpClient();
	void talk_to_server_tcp(sf::TcpSocket& socket);
	void talk_to_server_udp(sf::UdpSocket& socket);
	void die(const char *message);
};

