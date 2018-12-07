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
	boidSocket.unbind();
}

void Client::update(float dt)
{
	totalTime += dt;
	tickTimer += dt;

	// Update obstacle manager
	clientObstacleManager->update(dt);

	// Update boid manager
	clientBoidManager->update(dt, clientObstacleManager->getObstacles());

	if (tickTimer >= (1.0f / 64.0f))
	{
		tickTimer = 0.0f;
		
		receiveBoidInfo(boidSocket);
		receiveObstacleInfo(obstacleSocket);
	}
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
	if (boidSocket.bind(sf::Socket::AnyPort, SERVERIP) != sf::Socket::Done)
	{
		// error...
		die("bind failed");
	}

	// Set the client socket to non-blocking
	boidSocket.setBlocking(false);

	// bind the socket to a port
	if (obstacleSocket.bind(sf::Socket::AnyPort, SERVERIP) != sf::Socket::Done)
	{
		// error...
		die("bind failed");
	}

	// Set the client socket to non-blocking
	obstacleSocket.setBlocking(false);

	connectToUDPServer(boidSocket);
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

void Client::receiveBoidInfo(sf::UdpSocket & socket)
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
			BoidData boidData(clientBoidManager->getBoidFlock()[i].getBoidID(), clientBoidManager->getBoidFlock()[i].getPosition().x, clientBoidManager->getBoidFlock()[i].getPosition().y, clientBoidManager->getBoidFlock()[i].getBoidVelocity().x, clientBoidManager->getBoidFlock()[i].getBoidVelocity().y, clientBoidManager->getBoidFlock()[i].getFillColor().r, clientBoidManager->getBoidFlock()[i].getFillColor().g, clientBoidManager->getBoidFlock()[i].getFillColor().b, clientBoidManager->getBoidFlock()[i].getFillColor().a, totalTime);
			sendBoidPacket << boidData.ID;
			sendBoidPacket << boidData.positionX;
			sendBoidPacket << boidData.positionY;
			sendBoidPacket << boidData.velocityX;
			sendBoidPacket << boidData.velocityY;
			sendBoidPacket << boidData.redValue;
			sendBoidPacket << boidData.greenValue;
			sendBoidPacket << boidData.blueValue;
			sendBoidPacket << boidData.alphaValue;
			sendBoidPacket << boidData.time;
		}
		else if (clientBoidManager->getBoidFlock()[i].getBoidID() != -1)
		{
			BoidData boidData(clientBoidManager->getBoidFlock()[i].getBoidID(), clientBoidManager->getBoidFlock()[i].getPosition().x, clientBoidManager->getBoidFlock()[i].getPosition().y, clientBoidManager->getBoidFlock()[i].getBoidVelocity().x, clientBoidManager->getBoidFlock()[i].getBoidVelocity().y, clientBoidManager->getBoidFlock()[i].getFillColor().r, clientBoidManager->getBoidFlock()[i].getFillColor().g, clientBoidManager->getBoidFlock()[i].getFillColor().b, clientBoidManager->getBoidFlock()[i].getFillColor().a, totalTime);
			sendBoidPacket << boidData.ID;
			sendBoidPacket << boidData.positionX;
			sendBoidPacket << boidData.positionY;
			sendBoidPacket << boidData.velocityX;
			sendBoidPacket << boidData.velocityY;
			sendBoidPacket << boidData.redValue;
			sendBoidPacket << boidData.greenValue;
			sendBoidPacket << boidData.blueValue;
			sendBoidPacket << boidData.alphaValue;
			sendBoidPacket << boidData.time;
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
				//totalTime = connect.totalTime;
				clientID = connect.playerID;
			}
				break;
			case BoidCount:
			{
				int count;
				receivePacket >> count;

				boidMsgs.clear();
				for (int i = 0; i < count; i++)
				{
					BoidData boidData(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
					receivePacket >> boidData.ID;
					receivePacket >> boidData.positionX;
					receivePacket >> boidData.positionY;
					receivePacket >> boidData.velocityX;
					receivePacket >> boidData.velocityY;
					receivePacket >> boidData.redValue;
					receivePacket >> boidData.greenValue;
					receivePacket >> boidData.blueValue;
					receivePacket >> boidData.alphaValue;
					receivePacket >> boidData.time;
					boidMsgs.push_back(boidData);
				}

				if (boidMsgs.size() > clientBoidManager->getBoidFlock().size())
				{
					int messagesSize = boidMsgs.size();
					int vectorSize = clientBoidManager->getBoidFlock().size();
					int diff = messagesSize - vectorSize;
					for (int i = boidMsgs.size() - diff; i < boidMsgs.size(); i++)
					{
						clientBoidManager->addBoidToFlock(boidMsgs[i].ID, boidMsgs[i].positionX, boidMsgs[i].positionY, boidMsgs[i].velocityX, boidMsgs[i].velocityY, boidMsgs[i].redValue, boidMsgs[i].greenValue, boidMsgs[i].blueValue, boidMsgs[i].alphaValue);
					}
				}
				
				for (int i = 0; i < clientBoidManager->getBoidFlock().size(); i++)
				{
					for (int j = 0; j < boidMsgs.size(); j++)
					{
						if (clientBoidManager->getBoidFlock()[i].getBoidID() == -1)
						{									  
							clientBoidManager->getBoidFlock()[i].setBoidID(boidMsgs[i].ID);
							clientBoidManager->getBoidFlock()[i].setPosition(sf::Vector2f(boidMsgs[i].positionX, boidMsgs[i].positionY));
							clientBoidManager->getBoidFlock()[i].setBoidVelocity(sf::Vector2f(boidMsgs[i].velocityX, boidMsgs[i].velocityY));
							clientBoidManager->getBoidFlock()[i].setFillColor(sf::Color(boidMsgs[i].redValue, boidMsgs[i].greenValue, boidMsgs[i].blueValue, boidMsgs[i].alphaValue));
						}
					}
				}

				/*for (int i = 0; i < boidMsgs.size(); i++)
				{
					for (int j = 0; j < clientBoidManager->getBoidFlock().size(); j++)
					{
						if (boidMsgs[i].ID == clientBoidManager->getBoidFlock()[j].getBoidID())
						{
							clientBoidManager->getBoidFlock()[j].addMessage(boidMsgs[i]);
						}
					}
				}*/
				/*for (int i = 0; i < clientBoidManager->getBoidFlock().size(); i++)
				{
					clientBoidManager->getBoidFlock()[i].predictPosition(totalTime);
				}*/

				gotID = true;
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

void Client::receiveObstacleInfo(sf::UdpSocket & clientSocket)
{
	sf::Packet sendObstaclePacket;

	NumObstacles numberOfObstacles(clientObstacleManager->getObstacles().size());

	numberOfObstacles.messageType = Messages::ObstacleCount;

	sendObstaclePacket << numberOfObstacles.messageType;
	sendObstaclePacket << numberOfObstacles.numberOfObstacles;

	for (int i = 0; i < clientObstacleManager->getObstacles().size(); i++)
	{
		if (clientObstacleManager->getObstacles()[i].getID() == -1)
		{
			ObstacleData obstacleData(-1, clientObstacleManager->getObstacles()[i].getPosition().x, clientObstacleManager->getObstacles()[i].getPosition().y);
			sendObstaclePacket << obstacleData.ID;
			sendObstaclePacket << obstacleData.positionX;
			sendObstaclePacket << obstacleData.positionY;
		}
		else
		{
			ObstacleData obstacleData(clientObstacleManager->getObstacles()[i].getID(), clientObstacleManager->getObstacles()[i].getPosition().x, clientObstacleManager->getObstacles()[i].getPosition().y);
			sendObstaclePacket << obstacleData.ID;
			sendObstaclePacket << obstacleData.positionX;
			sendObstaclePacket << obstacleData.positionY;
		}
	}

	// UDP socket:
	sf::IpAddress recipient = SERVERIP;
	unsigned short port = SERVERPORT2;

	if (clientSocket.send(sendObstaclePacket, recipient, port) != sf::Socket::Done)
	{
		// error...
		die("sendto failed");
	}

	// Receive data
	sf::Packet receivePacket;

	// UDP socket:
	sf::IpAddress sender;
	unsigned short prt;

	if (clientSocket.receive(receivePacket, sender, prt) == sf::Socket::Done)
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
				obsMsgs.clear();
				for (int i = 0; i < count; i++)
				{
					ObstacleData obsData(0, 0, 0);
					receivePacket >> obsData.ID;
					receivePacket >> obsData.positionX;
					receivePacket >> obsData.positionY;
					obsMsgs.push_back(obsData);
				}

				if (clientObstacleManager->getObstacles().empty())
				{
					for (int i = 0; i < obsMsgs.size(); i++)
					{
						clientObstacleManager->addObstacle(obsMsgs[i].ID, obsMsgs[i].positionX, obsMsgs[i].positionY);
					}
				}

				if (obsMsgs.size() > clientObstacleManager->getObstacles().size())
				{
					int messagesSize = obsMsgs.size();
					int vectorSize = clientObstacleManager->getObstacles().size();
					int diff = messagesSize - vectorSize;
					for (int i = obsMsgs.size() - diff; i < obsMsgs.size(); i++)
					{
						clientObstacleManager->addObstacle(obsMsgs[i].ID, obsMsgs[i].positionX, obsMsgs[i].positionY);
					}
				}

				for (int j = 0; j < clientObstacleManager->getObstacles().size(); j++)
				{
					for (int k = 0; k < obsMsgs.size(); k++)
					{
						if (clientObstacleManager->getObstacles()[j].getPosition() == sf::Vector2f(obsMsgs[k].positionX, obsMsgs[k].positionY))
						{
							clientObstacleManager->getObstacles()[j].setID(obsMsgs[k].ID);
							clientObstacleManager->getObstacles()[j].setPosition(obsMsgs[k].positionX, obsMsgs[k].positionY);
						}
						if (clientObstacleManager->getObstacles()[j].getPosition() != sf::Vector2f(obsMsgs[k].positionX, obsMsgs[k].positionY) && clientObstacleManager->getObstacles()[j].getPosition() != clientObstacleManager->getObstacles()[j].getPosition())
						{
							clientObstacleManager->addObstacle(obsMsgs[k].ID, obsMsgs[k].positionX, obsMsgs[k].positionY);
						}
					}
				}
			}
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
