#include "Client.h"


Client::Client(sf::RenderWindow* hwnd, Input* in)
{
	window = hwnd;
	input = in;

	totalTime = 0.0f;
	clientID = 0;
	tickTimer = 0.0f;

	// Initialise new boid manager
	clientBoidManager = new BoidManager(window, input);

	// Initialise new obstacle manager
	clientObstacleManager = new ObstacleManager(window, input);

	udpClientSocketSetup();
}

Client::~Client()
{
	//// We won't actually get here, but if we did then we'd want to clean up...
	printf("Quitting\n");
	clientSocket.unbind();
}

void Client::update(float dt)
{
	totalTime += dt;
	tickTimer += dt;

	if (tickTimer >= (1.0f / 64.0f))
	{
		tickTimer = 0.0f;
		
		talk_to_server_udp(clientSocket);
	}

	// Update obstacle manager
	clientObstacleManager->update(dt);

	// Update boid manager
	clientBoidManager->update(dt, clientObstacleManager->getObstacles());
}

void Client::render(sf::RenderWindow * window)
{
	// Draw obstacles in obstacle manager
	clientObstacleManager->render(window);

	// Draw boids in boid manager
	clientBoidManager->render(window);
}

void Client::tcpClient()
{
	printf("Client TCP Program\n");

	// Initialise SFML TCP socket
	sf::TcpSocket socket;
	sf::Socket::Status status = socket.connect(SERVERIP, SERVERPORT);
	if (status != sf::Socket::Done)
	{
		// error...
		die("connect failed");
	}

	// Print the IP and Port we're connecting to.
	printf("IP address to connect to: %s\n", SERVERIP);
	printf("Port number to connect to: %d\n\n", SERVERPORT);

	printf("Connected to server\n");

	talk_to_server_tcp(socket);

	printf("Quitting\n");

	// Close the socket and clean up the sockets library.
	socket.disconnect();
}

void Client::udpClientSocketSetup()
{
	printf("Client UDP Program\n");

	// Print the IP and Port we're connecting to.
	printf("IP address to send to:: %s\n", SERVERIP);
	printf("Port number to send to: %d\n\n", SERVERPORT);

	// bind the socket to a port
	if (clientSocket.bind(sf::Socket::AnyPort, SERVERIP) != sf::Socket::Done)
	{
		// error...
		die("bind failed");
	}

	// Set the client socket to non-blocking
	clientSocket.setBlocking(false);

	connectToUDPServer(clientSocket);
}

void Client::connectToUDPServer(sf::UdpSocket & socket)
{
	sf::Packet connectPacket;

	NewConnection connectMsg;

	connectMsg.messageType = Messages::Connect;
	connectMsg.time = totalTime;
	connectMsg.totalTime = totalTime;
	connectMsg.playerID = clientID;

	connectPacket
		<< connectMsg.messageType
		<< connectMsg.time
		<< connectMsg.totalTime
		<< connectMsg.playerID;

	if (socket.send(connectPacket, SERVERIP, SERVERPORT) != sf::Socket::Done)
	{
		// error...
		die("sendto failed");
	}

	
}

void Client::talk_to_server_tcp(sf::TcpSocket & socket)
{
	// We'll use this buffer to hold what we receive from the server.
	char buffer[MESSAGESIZE];

	while (memcmp(buffer, "quit", 4) != 0)
	{
		printf("Type some text (\"quit\" to exit): ");
		fflush(stdout);

		// Read a line of text from the user.
		std::string line;
		std::getline(std::cin, line);
		int lineSize = line.size();
		// Now "line" contains what the user typed (without the trailing \n).

		// Copy the line into the buffer, filling the rest with dashes.
		// (We must be careful not to write past the end of the array.)
		memset(buffer, '-', MESSAGESIZE);
		memcpy(buffer, line.c_str(), std::min(lineSize, MESSAGESIZE));

		if (socket.send(buffer, MESSAGESIZE) != sf::Socket::Done)
		{
			// error...
			printf("send failed\n");
		}

		// Receive data
		std::size_t received;

		// TCP socket: (blocking)
		if (socket.receive(buffer, MESSAGESIZE, received) != sf::Socket::Done)
		{
			// error...
			die("receive failed");
		}
		std::cout << "Received " << received << " bytes: '";
		fwrite(buffer, 1, MESSAGESIZE, stdout);
		printf("'\n");
	}
}

void Client::talk_to_server_udp(sf::UdpSocket & socket)
{
	sf::Packet sendBoidPacket;

	NumBoids numberOfBoids(clientBoidManager->getBoidFlock().size());

	numberOfBoids.messageType = Messages::BoidCount;

	sendBoidPacket << numberOfBoids.messageType;
	sendBoidPacket << numberOfBoids.numberOfBoids;

	for (int i = 0; i < clientBoidManager->getBoidFlock().size(); i++)
	{
		if (clientBoidManager->getBoidFlock()[i].getBoidID() == -1 && !sentBoids)
		{
			BoidData boidData(clientBoidManager->getBoidFlock()[i].getBoidID(), clientBoidManager->getBoidFlock()[i].getPosition().x, clientBoidManager->getBoidFlock()[i].getPosition().y, clientBoidManager->getBoidFlock()[i].getBoidVelocity().x, clientBoidManager->getBoidFlock()[i].getBoidVelocity().y, clientBoidManager->getBoidFlock()[i].getFillColor().r, clientBoidManager->getBoidFlock()[i].getFillColor().g, clientBoidManager->getBoidFlock()[i].getFillColor().b, clientBoidManager->getBoidFlock()[i].getFillColor().a);
			sendBoidPacket << boidData.ID;
			sendBoidPacket << boidData.positionX;
			sendBoidPacket << boidData.positionY;
			sendBoidPacket << boidData.velocityX;
			sendBoidPacket << boidData.velocityY;
			sendBoidPacket << boidData.redValue;
			sendBoidPacket << boidData.greenValue;
			sendBoidPacket << boidData.blueValue;
			sendBoidPacket << boidData.alphaValue;
		}
		else if (clientBoidManager->getBoidFlock()[i].getBoidID() != -1)
		{
			BoidData boidData(clientBoidManager->getBoidFlock()[i].getBoidID(), clientBoidManager->getBoidFlock()[i].getPosition().x, clientBoidManager->getBoidFlock()[i].getPosition().y, clientBoidManager->getBoidFlock()[i].getBoidVelocity().x, clientBoidManager->getBoidFlock()[i].getBoidVelocity().y, clientBoidManager->getBoidFlock()[i].getFillColor().r, clientBoidManager->getBoidFlock()[i].getFillColor().g, clientBoidManager->getBoidFlock()[i].getFillColor().b, clientBoidManager->getBoidFlock()[i].getFillColor().a);
			sendBoidPacket << boidData.ID;
			sendBoidPacket << boidData.positionX;
			sendBoidPacket << boidData.positionY;
			sendBoidPacket << boidData.velocityX;
			sendBoidPacket << boidData.velocityY;
			sendBoidPacket << boidData.redValue;
			sendBoidPacket << boidData.greenValue;
			sendBoidPacket << boidData.blueValue;
			sendBoidPacket << boidData.alphaValue;
		}
	}

	sentBoids = true;
	
	// UDP socket:
	sf::IpAddress recipient = SERVERIP;
	unsigned short port = SERVERPORT;

	if (socket.send(sendBoidPacket, recipient, port) != sf::Socket::Done)
	{
		// error...
		die("sendto failed");
	}

	sf::Packet sendObstaclePacket;

	NumObstacles numberOfObstacles(clientObstacleManager->getObstacles().size());

	numberOfObstacles.messageType = Messages::ObstacleCount;

	sendBoidPacket << numberOfObstacles.messageType;
	sendBoidPacket << numberOfObstacles.numberOfObstacles;

	for (int i = 0; i < clientObstacleManager->getObstacles().size(); i++)
	{
		ObstacleData obstacleData(clientObstacleManager->getObstacles()[i].getPosition().x, clientObstacleManager->getObstacles()[i].getPosition().y);
		sendObstaclePacket << obstacleData.positionX;
		sendObstaclePacket << obstacleData.positionY;
	}

	if (socket.send(sendObstaclePacket, recipient, port) != sf::Socket::Done)
	{
		// error...
		die("sendto failed");
	}

	sf::Packet receivePacket;
	int msgType;

	// UDP socket:
	sf::IpAddress sender;
	unsigned short prt;

	if (socket.receive(receivePacket, sender, prt) == sf::Socket::Done)
	{
		std::cout << "GOT INFO." << std::endl;
		if (receivePacket >> msgType)
		{
			switch (msgType)
			{
			case Connect:
			{
				NewConnection connect(0, 0, 0);
				receivePacket >> connect.time;
				receivePacket >> connect.totalTime;
				receivePacket >> connect.playerID;
				totalTime = connect.totalTime;
				clientID = connect.playerID;
			}
				break;
			case BoidCount:
			{
				int count;
				receivePacket >> count;
				
				for (int i = 0; i < count; i++)
				{
					BoidData boidData(0, 0, 0, 0, 0,0,0,0,0);
					receivePacket >> boidData.ID;
					receivePacket >> boidData.positionX;
					receivePacket >> boidData.positionY;
					receivePacket >> boidData.velocityX;
					receivePacket >> boidData.velocityY;
					receivePacket >> boidData.redValue;
					receivePacket >> boidData.greenValue;
					receivePacket >> boidData.blueValue;
					receivePacket >> boidData.alphaValue;

					if (boidData.ID < 5)
					{
						// Server boids
						if (!gotID)
						{
							clientBoidManager->addBoidToFlock(boidData.ID, boidData.positionX, boidData.positionY, boidData.velocityX, boidData.velocityY, boidData.redValue, boidData.greenValue, boidData.blueValue, boidData.alphaValue);
						}
						else
						{
							clientBoidManager->getBoidFlock()[i].setBoidID(boidData.ID);
							clientBoidManager->getBoidFlock()[i].setPosition(sf::Vector2f(boidData.positionX, boidData.positionY));
							clientBoidManager->getBoidFlock()[i].setBoidVelocity(sf::Vector2f(boidData.velocityX, boidData.velocityY));
							clientBoidManager->getBoidFlock()[i].setFillColor(sf::Color(boidData.redValue, boidData.greenValue, boidData.blueValue, boidData.alphaValue));
						}
					}
					if (clientBoidManager->getBoidFlock()[i].getBoidID() == -1)
					{
						// This specific clients' boids
						clientBoidManager->getBoidFlock()[i].setBoidID(boidData.ID);
						clientBoidManager->getBoidFlock()[i].setPosition(sf::Vector2f(boidData.positionX, boidData.positionY));
						clientBoidManager->getBoidFlock()[i].setBoidVelocity(sf::Vector2f(boidData.velocityX, boidData.velocityY));
						clientBoidManager->getBoidFlock()[i].setFillColor(sf::Color(boidData.redValue, boidData.greenValue, boidData.blueValue, boidData.alphaValue));
					}	
					if (boidData.ID != clientBoidManager->getBoidFlock()[i].getBoidID())
					{
						if (!gotID)
						{
							clientBoidManager->addBoidToFlock(boidData.ID, boidData.positionX, boidData.positionY, boidData.velocityX, boidData.velocityY, boidData.redValue, boidData.greenValue, boidData.blueValue, boidData.alphaValue);
						}
					}
				}
				gotID = true;
				//// UDP socket:
				//sf::IpAddress recipient = SERVERIP;
				//if (clientSocket.send(sendPacket, recipient, port) != sf::Socket::Done)
				//{
				//	// error...
				//	die("sendto failed");
				//}
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
}

void Client::die(const char * message)
{
	fprintf(stderr, "Error: %s (WSAGetLastError() = %d)\n", message, sf::Socket::Status());

#ifdef _DEBUG
	// Debug build -- drop the program into the debugger.
	abort();
#else
	exit(1);
#endif
}
