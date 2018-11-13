#include "Server.h"


Server::Server()
{
}


Server::~Server()
{
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
void Server::udpServer()
{
	printf("Echo UDP SFML Server\n");

	sf::UdpSocket socket;

	// bind the socket to a port
	if (socket.bind(SERVERPORT, SERVERIP) != sf::Socket::Done)
	{
		// error...
		die("bind failed");
	}

	// Print IP and Port the server is bound to
	printf("Server socket bound to address %s, port %d\n", SERVERIP, SERVERPORT);

	while (true)
	{
		printf("Waiting for a message...\n");

		talk_to_client_udp(socket);
	}

	// We won't actually get here, but if we did then we'd want to clean up...
	printf("Quitting\n");
	socket.unbind();
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
	std::string s;

	while (s != "quit")
	{
		// Receive data
		sf::Packet packet;

		std::size_t received;

		// UDP socket:
		sf::IpAddress sender;
		unsigned short port;

		if (clientSocket.receive(packet, sender, port) != sf::Socket::Done)
		{
			// error...
			die("receive failed");
		}
		if (packet >> s)
		{
			// ok
			std::cout << "Received " << packet.getDataSize() << " bytes from " << sender << " on port " << port << std::endl;
			std::cout << "'" << s << "'" << std::endl;
		}
		else
		{
			// die
		}

		// UDP socket:
		sf::IpAddress recipient = SERVERIP;
		if (clientSocket.send(packet, recipient, port) != sf::Socket::Done)
		{
			// error...
			die("sendto failed");
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
