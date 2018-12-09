// Server.cpp
// Communicates with client about boids/obstacles

#include "Server.h"

Server::Server(sf::RenderWindow* hwnd, Input* in)
{
	// Set the window and input for the server
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

	// Initialise the total time and tick timer
	totalTime = 0.0f;
	tickTimer = 0.0f;

	// Set up the UDP sockets for communication
	udpServerSocketSetup();
}

Server::~Server()
{
	// Unbind all the sockets
	boidSocket.unbind();
	obstacleSocket.unbind();
}

void Server::update(float dt)
{
	// Increase total time and tick timer by delta time
	totalTime += dt;
	tickTimer += dt;

	// Update obstacle manager
	serverObstacleManager->update(dt);

	// Update boid manager
	serverBoidManager->update(dt, serverObstacleManager->getObstacles());

	// Every 64 ticks communicate with clients
	if (tickTimer >= 1.0f / 64.0f)
	{
		// Reset tick timer
		tickTimer = 0.0f;

		// Communicate boid information
		receiveBoidInfo(boidSocket);
		// Communicate obstacle information
		receiveObstacleInfo(obstacleSocket);
	}
}

void Server::render(sf::RenderWindow * window)
{
	// Draw obstacles in obstacle manager
	serverObstacleManager->render(window);

	// Draw boids in boid manager
	serverBoidManager->render(window);
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

void Server::receiveBoidInfo(sf::UdpSocket & clientSocket)
{
	// Packet to store information about boids
	sf::Packet receivePacket;

	// Address and port of the sender
	sf::IpAddress sender;
	unsigned short port;

	if (clientSocket.receive(receivePacket, sender, port) == sf::Socket::Done)
	{
		// Message type
		int msgType;
		
		// Check message type from packet is valid
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

					// Fill the packet with relevant information
					idPacket << connect.messageType;
					idPacket << connect.time;
					idPacket << connect.totalTime;
					idPacket << connect.playerID;

					// Send information back to the client
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
					// Number of boids information received
					int count;
					receivePacket >> count;

					// Clear the message history
					boidMsgs.clear();
					for (int i = 0; i < count; i++)
					{
						// Fill out a message with information from the packet
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

					// Check if ID's in the messages are -1, if so, they are new so add them to the servers flock and give them an ID
					for (int i = 0; i < boidMsgs.size(); i++)
					{
						if (boidMsgs[i].ID == -1)
						{
							serverBoidManager->addBoidToFlock(serverBoidManager->getBoidFlock().size() + i, boidMsgs[i].positionX, boidMsgs[i].positionY, boidMsgs[i].velocityX, boidMsgs[i].velocityY, boidMsgs[i].redValue, boidMsgs[i].greenValue, boidMsgs[i].blueValue, boidMsgs[i].alphaValue);
						}
					}

					// Loop through messages and server flock to check if ID's match
					// If so, received a message about the server flocks boid
					for (int j = 0; j < serverBoidManager->getBoidFlock().size(); j++)
					{
						for (int i = 0; i < boidMsgs.size(); i++)
						{
							if (boidMsgs[i].ID == serverBoidManager->getBoidFlock()[i].getBoidID())
							{
								serverBoidManager->getBoidFlock()[j].addMessage(boidMsgs[i]);
							}
						}
					}

					// Predict positions of boids
					for (int i = 0; i < serverBoidManager->getBoidFlock().size(); i++)
					{
						//serverBoidManager->getBoidFlock()[i].predictPosition(totalTime);
					}

					// Packet to send the information back to the client
					sf::Packet sendPacket;
					// Fill out message with boid count and type
					NumBoids numBoid(serverBoidManager->getBoidFlock().size());
					numBoid.messageType = Messages::BoidCount;
					// Fill out packet with message type and boid count
					sendPacket << numBoid.messageType;
					sendPacket << numBoid.numberOfBoids;

					// Loop through the servers flock and add that information to the packet
					for (int i = serverBoidManager->getBoidFlock().size() - 1; i > -1; i--)
					{
						BoidData boidData(serverBoidManager->getBoidFlock()[i].getBoidID(), serverBoidManager->getBoidFlock()[i].getPosition().x, serverBoidManager->getBoidFlock()[i].getPosition().y, serverBoidManager->getBoidFlock()[i].getBoidVelocity().x, serverBoidManager->getBoidFlock()[i].getBoidVelocity().y, serverBoidManager->getBoidFlock()[i].getFillColor().r, serverBoidManager->getBoidFlock()[i].getFillColor().g, serverBoidManager->getBoidFlock()[i].getFillColor().b, serverBoidManager->getBoidFlock()[i].getFillColor().a, totalTime);
						sendPacket << boidData.ID;
						sendPacket << boidData.positionX;
						sendPacket << boidData.positionY;
						sendPacket << boidData.velocityX;
						sendPacket << boidData.velocityY;
						sendPacket << boidData.redValue;
						sendPacket << boidData.greenValue;
						sendPacket << boidData.blueValue;
						sendPacket << boidData.alphaValue;
						sendPacket << boidData.time;
					}

					// Send boid information back to the client
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
	// Packet to receive information about obstacles
	sf::Packet receivePacket;

	// Address and port of sender
	sf::IpAddress sender;
	unsigned short port;

	if (clientSocket.receive(receivePacket, sender, port) == sf::Socket::Done)
	{
		// Type
		int msgType;

		// Check type is valid
		if (receivePacket >> msgType)
		{
			switch (msgType)
			{
			case ObstacleCount:
			{
				// Number of obstacle data we've received
				int count;
				receivePacket >> count;

				// Clear the message history
				obsMsgs.clear();
				for (int i = 0; i < count; i++)
				{
					// Fill out message with data from packet
					ObstacleData obsData(0, 0, 0);
					receivePacket >> obsData.ID;
					receivePacket >> obsData.positionX;
					receivePacket >> obsData.positionY;
					obsMsgs.push_back(obsData);
				}

				// If the server has no obstacles and received information about some
				// Add them to the servers obstacle manager
				if (serverObstacleManager->getObstacles().empty())
				{
					for (int i = 0; i < obsMsgs.size(); i++)
					{
						serverObstacleManager->addObstacle(obsMsgs[i].ID, obsMsgs[i].positionX, obsMsgs[i].positionY);
					}
				}

				// If there are more obstacles received than are on the server, add them
				if (obsMsgs.size() > serverObstacleManager->getObstacles().size())
				{
					int messagesSize = obsMsgs.size();
					int vectorSize = serverObstacleManager->getObstacles().size();
					int diff = messagesSize - vectorSize;
					for (int i = obsMsgs.size() - diff; i < obsMsgs.size(); i++)
					{
						serverObstacleManager->addObstacle(obsMsgs[i].ID, obsMsgs[i].positionX, obsMsgs[i].positionY);
					}
				}

				// Set ID and postion of obstacles
				for (int j = 0; j < serverObstacleManager->getObstacles().size(); j++)
				{
					for (int k = 0; k < obsMsgs.size(); k++)
					{
						if (serverObstacleManager->getObstacles()[j].getPosition() == sf::Vector2f(obsMsgs[k].positionX, obsMsgs[k].positionY))
						{
							serverObstacleManager->getObstacles()[j].setID(obsMsgs[k].ID);
							serverObstacleManager->getObstacles()[j].setPosition(obsMsgs[k].positionX, obsMsgs[k].positionY);
						}
						if (serverObstacleManager->getObstacles()[j].getPosition() != sf::Vector2f(obsMsgs[k].positionX, obsMsgs[k].positionY) && serverObstacleManager->getObstacles()[j].getPosition() != serverObstacleManager->getObstacles()[j].getPosition())
						{
							serverObstacleManager->addObstacle(obsMsgs[k].ID, obsMsgs[k].positionX, obsMsgs[k].positionY);
						}
					}
				}
				
				// Packet to send obstacle information back to the client
				sf::Packet sendObsPacket;
				// Fill out message with number of obstacles on the server and message type
				NumObstacles numberOfObstacles(serverObstacleManager->getObstacles().size());
				numberOfObstacles.messageType = Messages::ObstacleCount;

				// Fill out packet with message type and number of obstacles
				sendObsPacket << numberOfObstacles.messageType;
				sendObsPacket << numberOfObstacles.numberOfObstacles;

				// Loop thorugh servers obstacle manager, fill out messages with obstacle information
				// Fill packet with messages
				for (int i = 0; i < serverObstacleManager->getObstacles().size(); i++)
				{
					ObstacleData obstacleData(serverObstacleManager->getObstacles()[i].getID(), serverObstacleManager->getObstacles()[i].getPosition().x, serverObstacleManager->getObstacles()[i].getPosition().y);
					sendObsPacket << obstacleData.ID;
					sendObsPacket << obstacleData.positionX;
					sendObsPacket << obstacleData.positionY;
				}

				// Send packet to client
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
