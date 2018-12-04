#include "Server.h"


Server::Server(sf::RenderWindow* hwnd, Input* in)
{
	window = hwnd;
	input = in;

	// Initialise new boid manager
	serverBoidManager = new BoidManager(window, input);

	for (int i = 0; i < serverBoidManager->getBoidFlock().size(); i++)
	{
		serverBoidManager->getBoidFlock()[i].setBoidID(i);
	}

	allBoidManagers.push_back(serverBoidManager);

	// Initialise new obstacle manager
	serverObstacleManager = new ObstacleManager(window, input);

	allObstacleManagers.push_back(serverObstacleManager);

	udpServerSocketSetup();

	recv = false;
	tickTimer = 0.0f;
}


Server::~Server()
{
	// We won't actually get here, but if we did then we'd want to clean up...
	printf("Quitting\n");
	serverSocket.unbind();
}

void Server::update(float dt)
{
	tickTimer += dt;
	if (tickTimer >= 1.0f / 64.0f)
	{
		tickTimer = 0.0f;
		//printf("Waiting for a message...\n");
		talk_to_client_udp(serverSocket);
	}

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

	// UDP socket:
	sf::IpAddress sender;
	unsigned short port;

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
				std::cout << "Client: " << connections.size() << " has connected." << std::endl;
			}
			break;
			case BoidCount:
			{
				int count;
				receivePacket >> count;
				for (int i = 0; i < count; i++)
				{
					BoidData boidData(0, 0, 0, 0, 0);
					receivePacket >> boidData.ID;
					receivePacket >> boidData.positionX;
					receivePacket >> boidData.positionY;
					receivePacket >> boidData.velocityX;
					receivePacket >> boidData.velocityY;
					if (boidData.ID == -1)
					{
						serverBoidManager->addBoidToFlock(boidData.positionX, boidData.positionY, boidData.velocityX, boidData.velocityY);
					}
					else if (boidData.ID == serverBoidManager->getBoidFlock()[5 + i].getBoidID())
					{
						serverBoidManager->getBoidFlock()[5 + i].setPosition(sf::Vector2f(boidData.positionX, boidData.positionY));
						serverBoidManager->getBoidFlock()[5 + i].setBoidVelocity(sf::Vector2f(boidData.velocityX, boidData.velocityY));
					}
				}
				sf::Packet sendPacket;
				NumBoids numBoid(5);
				numBoid.messageType = Messages::BoidCount;

				sendPacket << numBoid.messageType;
				sendPacket << numBoid.numberOfBoids;

				for (int i = 0; i < 5; i++)
				{
					BoidData boidData(5 + i, serverBoidManager->getBoidFlock()[5 + i].getPosition().x, serverBoidManager->getBoidFlock()[5 + i].getPosition().y, serverBoidManager->getBoidFlock()[5 + i].getBoidVelocity().x, serverBoidManager->getBoidFlock()[5 + i].getBoidVelocity().y);
					sendPacket << boidData.ID;
					sendPacket << boidData.positionX;
					sendPacket << boidData.positionY;
					sendPacket << boidData.velocityX;
					sendPacket << boidData.velocityY;
				}

				// UDP socket:
				sf::IpAddress recipient = SERVERIP;
				if (clientSocket.send(sendPacket, recipient, port) != sf::Socket::Done)
				{
					// error...
					die("sendto failed");
				}
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
		//printf("Nothing received\n");
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
