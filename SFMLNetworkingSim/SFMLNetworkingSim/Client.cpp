// Client.cpp
// Has its own boid flock
// Communicates with the server

#include "Client.h"

Client::Client(sf::RenderWindow* hwnd, Input* in)
{
	// Set the window and input of the client
	window = hwnd;
	input = in;

	// Initialise total time, ID and tick timer to 0
	totalTime = 0.0f;
	clientID = 0;
	tickTimer = 0.0f;

	// Initialise new boid manager
	clientBoidManager = new BoidManager(window, input);

	// Initialise new obstacle manager
	clientObstacleManager = new ObstacleManager(window, input);

	// Set up the UDP sockets the client will use
	udpClientSocketSetup();
}

Client::~Client()
{
	// Unbind the clients sockets
	boidSocket.unbind();
	obstacleSocket.unbind();
}

void Client::update(float dt)
{
	// Increase the total time and tick timer by delta time
	totalTime += dt;
	tickTimer += dt;

	// Update obstacle manager
	clientObstacleManager->update(dt);

	// Update boid manager
	clientBoidManager->update(dt, clientObstacleManager->getObstacles());

	// Send/Receive infor every 64 ticks
	if (tickTimer >= (1.0f / 64.0f))
	{
		tickTimer = 0.0f;
		
		// Send/Receive boid information
		receiveBoidInfo(boidSocket);
		// Send/Receive obstacle information
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

	// Attempt a connection with UDP sockets to the server
	connectToUDPServer(boidSocket);
}

void Client::connectToUDPServer(sf::UdpSocket & socket)
{
	// Packet to contain all client info for the server
	sf::Packet connectPacket;

	// Message type the packet will contain
	NewConnection connectMsg;

	// Fill out the message for the packet
	connectMsg.messageType = Messages::Connect;
	connectMsg.time = totalTime;
	connectMsg.totalTime = totalTime;
	connectMsg.playerID = clientID;

	// Fill the packet with the message data
	connectPacket
		<< connectMsg.messageType
		<< connectMsg.time
		<< connectMsg.totalTime
		<< connectMsg.playerID;

	// Send and attempt to connect to the server
	if (socket.send(connectPacket, SERVERIP, SERVERPORT) != sf::Socket::Done)
	{
		// error...
		die("sendto failed");
	}
}

void Client::receiveBoidInfo(sf::UdpSocket & socket)
{
	// Packet to contain all the boid info for the server
	sf::Packet sendBoidPacket;

	// Message type the packet will contain
	NumBoids numberOfBoids(clientBoidManager->getBoidFlock().size());

	// Number of boid information the packet will contain
	numberOfBoids.messageType = Messages::BoidCount;

	// Fill the packet with the message type and number of boids
	sendBoidPacket << numberOfBoids.messageType;
	sendBoidPacket << numberOfBoids.numberOfBoids;

	for (int i = 0; i < clientBoidManager->getBoidFlock().size(); i++)
	{
		if (clientBoidManager->getBoidFlock()[i].getBoidID() == -1 && !sentBoids)
		{
			// Fill out new message type containing the boids ID, position, velocity, colour and time the message was sent
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
			// Fill out new message type containing the boids ID, position, velocity, colour and time the message was sent
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

	// Address and port of the server to send the information to
	sf::IpAddress recipient = SERVERIP;
	unsigned short port = SERVERPORT;

	// Send the boid packet to the server
	if (socket.send(sendBoidPacket, recipient, port) != sf::Socket::Done)
	{
		// error...
		die("sendto failed");
	}

	// Packet to contain all the boid info from the server
	sf::Packet receivePacket;
	// Message type that will be filled by the server
	int msgType;

	// Address and port the server will send from
	sf::IpAddress sender;
	unsigned short prt;

	// Check for messages from the server
	if (socket.receive(receivePacket, sender, prt) == sf::Socket::Done)
	{
		// Check that the message type is valid
		if (receivePacket >> msgType)
		{
			// Switch statement based on the messages type
			switch (msgType)
			{
			case Connect:
			{
				// Fill out a new connect message
				// Set this clients time and ID
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
				// The number of boids information that the packet will contain
				int count;
				// Fill count with the packet data
				receivePacket >> count;

				// Clear the message history
				boidMsgs.clear();
				for (int i = 0; i < count; i++)
				{
					// Fill out a boid data message with information from the packet
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

				// Check if there are more messages than boids in the clients flock
				if (boidMsgs.size() > clientBoidManager->getBoidFlock().size())
				{
					int messagesSize = boidMsgs.size();
					int vectorSize = clientBoidManager->getBoidFlock().size();
					int diff = messagesSize - vectorSize;
					for (int i = boidMsgs.size() - diff; i < boidMsgs.size(); i++)
					{
						// Add a new boid to the clients flock
						clientBoidManager->addBoidToFlock(boidMsgs[i].ID, boidMsgs[i].positionX, boidMsgs[i].positionY, boidMsgs[i].velocityX, boidMsgs[i].velocityY, boidMsgs[i].redValue, boidMsgs[i].greenValue, boidMsgs[i].blueValue, boidMsgs[i].alphaValue);
					}
				}
				
				// Update the client boids ID, position, velocity and colour once verified by the server
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
	// Packet to contain all the obstacle info for the server
	sf::Packet sendObstaclePacket;

	// Fill out a new message with how many obstacles this client has
	NumObstacles numberOfObstacles(clientObstacleManager->getObstacles().size());

	// Set the message type
	numberOfObstacles.messageType = Messages::ObstacleCount;

	// Fill the packet with the type and number of obstacles this client has
	sendObstaclePacket << numberOfObstacles.messageType;
	sendObstaclePacket << numberOfObstacles.numberOfObstacles;

	// Loop through all the obstacles this client has
	for (int i = 0; i < clientObstacleManager->getObstacles().size(); i++)
	{
		// If its ID is -1, usually a fresh obstacle
		if (clientObstacleManager->getObstacles()[i].getID() == -1)
		{
			ObstacleData obstacleData(-1, clientObstacleManager->getObstacles()[i].getPosition().x, clientObstacleManager->getObstacles()[i].getPosition().y);
			sendObstaclePacket << obstacleData.ID;
			sendObstaclePacket << obstacleData.positionX;
			sendObstaclePacket << obstacleData.positionY;
		}
		// Otherwise it has been verified by the server
		else
		{
			ObstacleData obstacleData(clientObstacleManager->getObstacles()[i].getID(), clientObstacleManager->getObstacles()[i].getPosition().x, clientObstacleManager->getObstacles()[i].getPosition().y);
			sendObstaclePacket << obstacleData.ID;
			sendObstaclePacket << obstacleData.positionX;
			sendObstaclePacket << obstacleData.positionY;
		}
	}

	// Address and port of the server to send that packet to
	sf::IpAddress recipient = SERVERIP;
	unsigned short port = SERVERPORT2;

	// Send the packet to the server
	if (clientSocket.send(sendObstaclePacket, recipient, port) != sf::Socket::Done)
	{
		// error...
		die("sendto failed");
	}

	// Packet to receive all obstacle data from the server
	sf::Packet receivePacket;

	// Address and port of the server to receive the packet from
	sf::IpAddress sender;
	unsigned short prt;

	// Receive a packet
	if (clientSocket.receive(receivePacket, sender, prt) == sf::Socket::Done)
	{
		// Type to check what the message type is
		int msgType;

		// Check the type is valid
		if (receivePacket >> msgType)
		{
			// Switch based on the type
			switch (msgType)
			{
			case ObstacleCount:
			{
				// How many obstacle's data has been received
				int count;
				// Fill count out from the packet
				receivePacket >> count;
				// Clear the message history
				obsMsgs.clear();
				for (int i = 0; i < count; i++)
				{
					// Fill out obstacle data from the packet
					ObstacleData obsData(0, 0, 0);
					receivePacket >> obsData.ID;
					receivePacket >> obsData.positionX;
					receivePacket >> obsData.positionY;
					obsMsgs.push_back(obsData);
				}

				// If there are no obstacles in the client simulation, add ones received from the server
				if (clientObstacleManager->getObstacles().empty())
				{
					for (int i = 0; i < obsMsgs.size(); i++)
					{
						clientObstacleManager->addObstacle(obsMsgs[i].ID, obsMsgs[i].positionX, obsMsgs[i].positionY);
					}
				}

				// If the information received has more obstacles than the client does,
				// Add the obstacle to the client simulation
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

				// Update position and ID of all obstacles verified by the server
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
