#include "Server.h"


Server::Server(sf::RenderWindow* hwnd, Input* in)
{
	window = hwnd;
	input = in;

	// Initialise new boid manager
	serverBoidManager = new BoidManager(window, input);

	allBoidManagers.push_back(serverBoidManager);

	// Initialise new obstacle manager
	serverObstacleManager = new ObstacleManager(window, input);

	allObstacleManagers.push_back(serverObstacleManager);

	udpServerSocketSetup();
}


Server::~Server()
{
	// We won't actually get here, but if we did then we'd want to clean up...
	printf("Quitting\n");
	serverSocket.unbind();
}

void Server::update(float dt)
{
	// Update obstacle manager
	for (auto obsManagers : allObstacleManagers)
	{
		obsManagers->update(dt);		
	}

	// Update boid manager
	for (auto boidManagers : allBoidManagers)
	{
		for (auto obsManagers : allObstacleManagers)
		{
			boidManagers->update(dt, obsManagers->getObstacles());
		}
	}

	printf("Waiting for a message...\n");
	talk_to_client_udp(serverSocket);
}

void Server::render(sf::RenderWindow * window)
{
	// Draw obstacles in obstacle manager
	for (auto obsManagers : allObstacleManagers)
	{
		obsManagers->render(window);
	}

	// Draw boids in boid manager
	for (auto boidManagers : allBoidManagers)
	{
		boidManagers->render(window);
	}
}

void Server::tcpServer()
{
	printf("Echo TCP SFML Server\n");

	// Initialise SFML TCP listener
	sf::TcpListener listener;

	// bind the listener to a port
	if (listener.listen(SERVERPORT, SERVERIP) != sf::Socket::Done)
	{
		// error...
		die("bind failed");
	}

	// Print IP and Port the server is bound to
	printf("Server socket bound to address %s, port %d\n", SERVERIP, SERVERPORT);

	printf("Server socket listening\n");

	while (true)
	{
		printf("Waiting for a connection...\n");

		// accept a new connection
		sf::TcpSocket client;
		if (listener.accept(client) != sf::Socket::Done)
		{
			// accept failed -- just try again.
			continue;
		}

		// use "client" to communicate with the connected client,
		// and continue to accept new connections with the listener
		printf("Client has connected from IP address %d, port %d!\n", client.getRemoteAddress(), client.getRemotePort());

		talk_to_client_tcp(client);

		printf("Client disconnected\n");

		// Close the connection.
		client.disconnect();
	}

	// We won't actually get here, but if we did then we'd want to clean up...
	printf("Quitting\n");
	listener.close();
}

// Run a server using UDP sockets.
void Server::udpServerSocketSetup()
{
	printf("Echo UDP SFML Server\n");

	// bind the socket to a port
	if (serverSocket.bind(SERVERPORT, SERVERIP) != sf::Socket::Done)
	{
		// error...
		die("bind failed");
	}

	// Set the server socket to non-blocking
	serverSocket.setBlocking(false);

	// Print IP and Port the server is bound to
	printf("Server socket bound to address %s, port %d\n", SERVERIP, SERVERPORT);
}

void Server::talk_to_client_tcp(sf::TcpSocket & clientSocket)
{
	char buffer[MESSAGESIZE];

	while (memcmp(buffer, "quit", 4) != 0)
	{
		// Receive data
		std::size_t received;

		// TCP socket: (blocking)
		if (clientSocket.receive(buffer, MESSAGESIZE, received) != sf::Socket::Done)
		{
			// error...
		}
		if (received == 0)
		{
			break;
		}

		std::cout << "Received " << received << " bytes: '";
		fwrite(buffer, 1, MESSAGESIZE, stdout);
		printf("'\n");

		// TCP socket:
		if (received > 0 && received <= MESSAGESIZE)
		{
			if (clientSocket.send(buffer, MESSAGESIZE) != sf::Socket::Done)
			{
				// error...
				printf("send failed\n");
				return;
			}
		}
	}
}

void Server::talk_to_client_udp(sf::UdpSocket & clientSocket)
{
	// Receive data
	sf::Packet receivePacket;

	std::size_t received;

	// UDP socket:
	sf::IpAddress sender;
	unsigned short port = SERVERPORT;

	if (clientSocket.receive(receivePacket, sender, port) == sf::Socket::Done)
	{

		int msgType;
		
		if (receivePacket >> msgType)
		{
			switch (msgType)
			{
			case Connect:
			{
				Connection * conn = new Connection();
				conn->connectionAddress = SERVERIP;
				conn->connectionPort = port;
				conn->ID = connections.size();

				connections.push_back(conn);
			}
			break;
			case BoidCount:
				int count;
				receivePacket >> count;
				for (int i = 0; i < count; i++)
				{
					BoidData boidData(0,0,0,0);
					receivePacket >> boidData.positionX;
					receivePacket >> boidData.positionY;
					receivePacket >> boidData.velocityX;
					receivePacket >> boidData.velocityY;
					serverBoidManager->addBoidToFlock(boidData.positionX, boidData.positionY, boidData.velocityX, boidData.velocityY);
				}
				break;
			case ObstacleCount:
				break;
			case ObstaclePositionData:
				break;
			case Disconnect:
				break;
			default:
				break;
			}
		}
	}
	else
	{
		printf("Nothing received\n");
	}

	// UDP socket:
	sf::IpAddress recipient = SERVERIP;
	if (clientSocket.send(receivePacket, recipient, port) != sf::Socket::Done)
	{
		// error...
		die("sendto failed");
	}
}

void Server::die(const char * message)
{
	fprintf(stderr, "Error: %s (WSAGetLastError() = %d)\n", message, sf::Socket::Status());

#ifdef _DEBUG
	// Debug build -- drop the program into the debugger.
	abort();
#else
	exit(1);
#endif
}
