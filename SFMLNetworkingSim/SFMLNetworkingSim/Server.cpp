#include "Server.h"


Server::Server(sf::RenderWindow* hwnd, Input* in)
{
	window = hwnd;
	input = in;

	// Initialise new boid manager
	serverBoidManager = new BoidManager(window, input);

	// Set server boid ID's to be 0 - 4.
	for (int i = 0; i < serverBoidManager->getBoidFlock().size(); i++)
	{
		serverBoidManager->getBoidFlock()[i].setBoidID(i);
		serverBoidManager->getBoidFlock()[i].setFillColor(sf::Color::White);
	}

	// Initialise new obstacle manager
	serverObstacleManager = new ObstacleManager(window, input);

	udpServerSocketSetup();
	tickTimer = 0.0f;
}

Server::~Server()
{
	// We won't actually get here, but if we did then we'd want to clean up...
	printf("Quitting\n");
	boidSocket.unbind();
}

void Server::update(float dt)
{
	// Increase tick timer
	tickTimer += dt;
	if (tickTimer >= 1.0f / 64.0f)
	{
		// Reset tick timer
		tickTimer = 0.0f;

		// Communicate with clients
		receiveBoidInfo(boidSocket);
		receiveObstacleInfo(obstacleSocket);
	}

	// Update obstacle manager
	serverObstacleManager->update(dt);

	// Update boid manager
	serverBoidManager->update(dt, serverObstacleManager->getObstacles());
}

void Server::render(sf::RenderWindow * window)
{
	// Draw obstacles in obstacle manager
	serverObstacleManager->render(window);

	// Draw boids in boid manager
	serverBoidManager->render(window);
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
	if (boidSocket.bind(SERVERPORT, SERVERIP) != sf::Socket::Done)
	{
		// error...
		die("bind failed");
	}

	// Set the server socket to non-blocking
	boidSocket.setBlocking(false);

	// bind the socket to a port
	if (obstacleSocket.bind(SERVERPORT2, SERVERIP) != sf::Socket::Done)
	{
		// error...
		die("bind failed");
	}

	// Set the server socket to non-blocking
	obstacleSocket.setBlocking(false);

	// Print IP and Port the server is bound to
	printf("Boid socket bound to address %s, port %d\n", SERVERIP, SERVERPORT);

	// Print IP and Port the server is bound to
	printf("Obstacle socket bound to address %s, port %d\n", SERVERIP, SERVERPORT2);
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

void Server::receiveBoidInfo(sf::UdpSocket & clientSocket)
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
					// Add new connection
					Connection * conn = new Connection(window, input);
					conn->connectionAddress = SERVERIP;
					conn->connectionPort = port;
					conn->ID = connections.size();

					// Add connection to vector of connections
					connections.push_back(conn);
					std::cout << "Client: " << connections.size() << " has connected." << std::endl;

					// Send ID and time information back to client
					sf::Packet idPacket;
					NewConnection connect(totalTime, tickTimer, connections.size());
					connect.messageType = Messages::Connect;

					idPacket << connect.messageType;
					idPacket << connect.time;
					idPacket << connect.totalTime;
					idPacket << connect.playerID;

					// UDP socket:
					sf::IpAddress recipient = SERVERIP;
					if (clientSocket.send(idPacket, recipient, port) != sf::Socket::Done)
					{
						// error...
						die("sendto failed");
					}
				}
				break;
				case BoidCount:
				{
					int count;
					receivePacket >> count;
					for (int i = 0; i < count; i++)
					{
						BoidData boidData(0, 0, 0, 0, 0, 0, 0, 0, 0);
						receivePacket >> boidData.ID;
						receivePacket >> boidData.positionX;
						receivePacket >> boidData.positionY;
						receivePacket >> boidData.velocityX;
						receivePacket >> boidData.velocityY;
						receivePacket >> boidData.redValue;
						receivePacket >> boidData.greenValue;
						receivePacket >> boidData.blueValue;
						receivePacket >> boidData.alphaValue;

						if (boidData.ID == -1)
						{
							serverBoidManager->addBoidToFlock(serverBoidManager->getBoidFlock().size() + i, boidData.positionX, boidData.positionY, boidData.velocityX, boidData.velocityY, boidData.redValue, boidData.greenValue, boidData.blueValue, boidData.alphaValue);
						}
						else if (boidData.ID == serverBoidManager->getBoidFlock()[i].getBoidID())
						{
							serverBoidManager->getBoidFlock()[i].setPosition(sf::Vector2f(boidData.positionX, boidData.positionY));
							serverBoidManager->getBoidFlock()[i].setBoidVelocity(sf::Vector2f(boidData.velocityX, boidData.velocityY));
						}
						/*else if (boidData.ID != serverBoidManager->getBoidFlock()[i].getBoidID())
						{
							serverBoidManager->addBoidToFlock(serverBoidManager->getBoidFlock().size() + i, boidData.positionX, boidData.positionY, boidData.velocityX, boidData.velocityY, boidData.redValue, boidData.greenValue, boidData.blueValue, boidData.alphaValue);
						}*/
					}
					sf::Packet sendPacket;
					NumBoids numBoid(serverBoidManager->getBoidFlock().size());
					numBoid.messageType = Messages::BoidCount;

					sendPacket << numBoid.messageType;
					sendPacket << numBoid.numberOfBoids;

					for (int i = serverBoidManager->getBoidFlock().size() - 1; i > -1; i--)
					{
						BoidData boidData(serverBoidManager->getBoidFlock()[i].getBoidID(), serverBoidManager->getBoidFlock()[i].getPosition().x, serverBoidManager->getBoidFlock()[i].getPosition().y, serverBoidManager->getBoidFlock()[i].getBoidVelocity().x, serverBoidManager->getBoidFlock()[i].getBoidVelocity().y, serverBoidManager->getBoidFlock()[i].getFillColor().r, serverBoidManager->getBoidFlock()[i].getFillColor().g, serverBoidManager->getBoidFlock()[i].getFillColor().b, serverBoidManager->getBoidFlock()[i].getFillColor().a);
						sendPacket << boidData.ID;
						sendPacket << boidData.positionX;
						sendPacket << boidData.positionY;
						sendPacket << boidData.velocityX;
						sendPacket << boidData.velocityY;
						sendPacket << boidData.redValue;
						sendPacket << boidData.greenValue;
						sendPacket << boidData.blueValue;
						sendPacket << boidData.alphaValue;
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
				case Disconnect:
					break;
				default:
					break;
			}
		}
	}
}

void Server::receiveObstacleInfo(sf::UdpSocket & clientSocket)
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
			case ObstacleCount:
			{
				int count;
				receivePacket >> count;
				for (int i = 0; i < count; i++)
				{
					ObstacleData obsData(-1, 0, 0);
					receivePacket >> obsData.ID;
					receivePacket >> obsData.positionX;
					receivePacket >> obsData.positionY;
					if (serverObstacleManager->getObstacles().size() < count)
					{
						if (obsData.ID == -1)
						{
							serverObstacleManager->addObstacle(i, obsData.positionX + 10, obsData.positionY + 10);
						}
						if (obsData.ID == serverObstacleManager->getObstacles()[i].getID())
						{
							serverObstacleManager->getObstacles()[i].setID(obsData.ID);
							serverObstacleManager->getObstacles()[i].setPosition(obsData.positionX, obsData.positionY);
						}
						/*else
						{
							serverObstacleManager->getObstacles()[i].setPosition(obsData.positionX, obsData.positionY);
						}*/
					}
				}
				sf::Packet sendObsPacket;
				NumObstacles numberOfObstacles(serverObstacleManager->getObstacles().size());

				numberOfObstacles.messageType = Messages::ObstacleCount;

				sendObsPacket << numberOfObstacles.messageType;
				sendObsPacket << numberOfObstacles.numberOfObstacles;

				for (int i = 0; i < serverObstacleManager->getObstacles().size(); i++)
				{
					ObstacleData obstacleData(serverObstacleManager->getObstacles()[i].getID(), serverObstacleManager->getObstacles()[i].getPosition().x, serverObstacleManager->getObstacles()[i].getPosition().y);
					sendObsPacket << obstacleData.ID;
					sendObsPacket << obstacleData.positionX;
					sendObsPacket << obstacleData.positionY;
				}

				// UDP socket:
				sf::IpAddress recipient = SERVERIP;
				if (clientSocket.send(sendObsPacket, recipient, port) != sf::Socket::Done)
				{
					// error...
					die("sendto failed");
				}
			}
			break;
			default:
				break;
			}
		}
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
