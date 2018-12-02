#include "Client.h"


Client::Client(sf::RenderWindow* hwnd, Input* in)
{
	window = hwnd;
	input = in;

	totalTime = 0.0f;
	clientID = 0;

	// Initialise new boid manager
	clientBoidManager = new BoidManager(window, input);

	allBoidManagers.push_back(clientBoidManager);

	// Initialise new obstacle manager
	clientObstacleManager = new ObstacleManager(window, input);

	allObstacleManagers.push_back(clientObstacleManager);

	udpClientSocketSetup();
}

Client::~Client()
{
	//// We won't actually get here, but if we did then we'd want to clean up...
	//printf("Quitting\n");
	//clientSocket.unbind();
}

void Client::update(float dt)
{
	talk_to_server_udp(clientSocket);

	totalTime += dt;

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

void Client::render(sf::RenderWindow * window)
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
	sf::Packet sendPacket;

	NumBoids numberOfBoids(clientBoidManager->getBoidFlock().size());

	numberOfBoids.messageType = Messages::BoidCount;

	sendPacket << numberOfBoids.messageType;
	sendPacket << numberOfBoids.numberOfBoids;

	for (int i = 0; i < clientBoidManager->getBoidFlock().size(); i++)
	{
		BoidData boidData(clientBoidManager->getBoidFlock()[i].getPosition().x, clientBoidManager->getBoidFlock()[i].getPosition().y, clientBoidManager->getBoidFlock()[i].getBoidVelocity().x, clientBoidManager->getBoidFlock()[i].getBoidVelocity().y);
		sendPacket << boidData.positionX;
		sendPacket << boidData.positionY;
		sendPacket << boidData.velocityX;
		sendPacket << boidData.velocityY;
	}
	
	// UDP socket:
	sf::IpAddress recipient = SERVERIP;
	unsigned short port = SERVERPORT;

	if (socket.send(sendPacket, recipient, port) != sf::Socket::Done)
	{
		// error...
		die("sendto failed");
	}

	//// Simulate packet drop
	//// Lose 40% of packets
	//if ((rand() % 100) > 40)
	//{
	//	
	//}
	// Receive data
	std::size_t received;

	// UDP socket:
	sf::IpAddress sender;
	unsigned short prt;

	//if (socket.receive(sendPacket, sender, prt) != sf::Socket::Done)
	//{
	//	// error...
	//	die("receive failed");
	//}
	////if (packet >> clientData.allBoidManagers.begin()._Ptr->_Myval->getBoidFlock().begin()._Ptr->_Myval.setPosition()
	////{
	////	// ok
	////	response_received = true;
	////	std::cout << "Received " << packet.getDataSize() << " bytes from " << sender << " on port " << prt << std::endl;
	////	std::cout << "'" << s << "'" << std::endl;
	////}
	//else
	//{
	//	// die
	//	std::cout << "packet broke yo" << std::endl;
	//}
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
