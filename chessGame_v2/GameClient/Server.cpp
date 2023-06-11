#include "Server.h"

void Server::GetLineFromCin_t(std::string* mssg, bool* exit) {
	while (*exit) {
		std::string line;
		std::getline(std::cin, line);
		mssg->assign(line);
	}
}

void Server::receive_and_return(std::vector<sf::TcpSocket*>* sockets, /*sf::Packet p,*/ std::string* mssg, unsigned int* port, bool* exit) {
	std::string mssg_temp;
	//std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	sf::Packet p;
	p << "A";
	p.clear();
	while (*exit) {
		mtx.lock();
		for (sf::TcpSocket* sock : *sockets) {
			sock->receive(p);
			p >> mssg_temp;
			if (mssg_temp.size() > 0) {
				p.clear();
				//std::cout << "Message Received Sv: ";
				*port = sock->getRemotePort();
				std::string s_port = std::to_string(*port);
				mssg_temp = s_port + ":" + mssg_temp;
				mssg->assign(mssg_temp);
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
		mtx.unlock();
	}
}

void Server::send_pkt_sv(std::vector<sf::TcpSocket*>* sockets, std::string mssg, unsigned int* port) {
	sf::Packet p;
	p << mssg;
	mtx.lock();
	for (sf::TcpSocket* sock : *sockets) {

		if (sock->getRemotePort() != *port) {
			sf::Socket::Status status = sock->send(p);
		}
		//sf::Socket::Status status = sock->send(p);
		//if (status != sf::Socket::Done) {
		//	// Error when sending data
		//	std::cout << "Error when sending data" << std::endl;
		//}

	}
	mtx.unlock();
	p.clear();
}

void Server::acceptSocket(sf::TcpListener* dispatcher, std::vector<sf::TcpSocket*>* sockets, bool* exit) {
	//Al conectarse un cliente, el socket incoming pasa a ser el que utilizan este cliente y el servidor para comunicarse en exclusiva
	dispatcher->listen(5000);
	unsigned int port;
	while (*exit) {
		sf::TcpSocket* sock = new sf::TcpSocket;
		if (dispatcher->accept(*sock) == sf::Socket::Done)
		{
			sock->setBlocking(false);
			mtx.lock();
			sockets->push_back(sock);
			mtx.unlock();
			std::cout << "Acepta conexion" << std::endl;
			std::cout << sockets->size() << std::endl;
			port = sock->getRemotePort();
			std::string s_port = std::to_string(port);
			std::string s = "Connected " + s_port + " waiting for opponent";
			welcome(sock, s);
		}
		//delete sock;
	}
}

void Server::welcome(sf::TcpSocket* sock, std::string mssg) {
	sf::Packet p;
	p << mssg;
	sf::Socket::Status status = sock->send(p);
}

void Server::ServerMain() 
{
	// Server mode
	std::cout << "Server mode running" << std::endl;

	bool exit = true;

	// We open the port 5000 manage TCP connection
	sf::TcpListener dispatcher;
	sf::Socket::Status status = dispatcher.listen(5000);
	if (status != sf::Socket::Done)
	{
		// If not connected sucessfully 
	}

	//When accepting a connection, we use the object socket. Once the connection is accepted, 
	//we can use this object to send and receive data to/from the client.
	//if (dispatcher.accept(socket) != sf::Socket::Done)
	//{
	//	//Error when accepting the connection
	//}
	//std::cout << "Connection accepted" << std::endl;
	std::thread accept_socket(&Server::acceptSocket, this, &dispatcher, &sockets, &exit);
	accept_socket.detach();

	// We create two packet objects, one for sending and another for receiving data.
	// Additionally, we initialize two string that will be used as message containers.
	sf::Packet inPacket, outPacket;
	std::string sendMessage, rcvMessage;
	unsigned int port;


	inPacket << "A"; //Inicializamos los paquetes
	inPacket.clear();

	outPacket << "A"; //Inicializamos los paquetes
	outPacket.clear();

	// Threads
	std::thread rcv_t(&Server::receive_and_return, this, &sockets, /*inPacket,*/ &rcvMessage, &port, &exit);
	rcv_t.detach();
	std::thread read_console_t(&Server::GetLineFromCin_t, this, &sendMessage, &exit);
	read_console_t.detach();

	// Application loop
	while (true) {
		// Logic for receiving
		if (rcvMessage.size() > 0) {
			if (rcvMessage == "exit") {
				//Gestionar la desconexion
				break;
			}
			std::cout << rcvMessage << std::endl;;
			sendMessage = rcvMessage;
			rcvMessage.clear();
		}
		// Logic for sending
		// std::cout << "In applicacion loop" << std::endl;
		if (sendMessage.size() > 0) {
			if (sendMessage == "exit") {
				// Desconection
				exit = false;
				sendMessage.clear();
				break;
			}
			else {
				send_pkt_sv(&sockets, sendMessage, &port);
				sendMessage.clear();
			}
		}
	}

	// When the application loop is broken, we have to release resources.
	dispatcher.close();
	mtx.lock();
	for (int i = 0; i < sockets.size(); i++) {
		sockets[i]->disconnect();
	}
	mtx.unlock();
	//accept_socket.join();
	//rcv_t.join();
	//read_console_t.join();
}
