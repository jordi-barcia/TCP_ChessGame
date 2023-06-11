#include "Client.h"

void Client::GetLineFromCin_t(std::string* mssg, bool* exit) {
	while (*exit) {
		std::string line;
		std::getline(std::cin, line);
		mssg->assign(line);
	}
}

void Client::send_pkt(sf::TcpSocket* sock, std::string mssg) {
	sf::Packet p;
	std::string mssg_temp;
	p << mssg;
	sf::Socket::Status status = sock->send(p);

	if (status != sf::Socket::Done) {
		// Error when sending data
		std::cout << "Error when sending data" << std::endl;
	}
	//else {
	//	std::cout << "Message sent:" << mssg << std::endl;
	//}
	p.clear();
}

void Client::receive_and_return_cl(sf::TcpSocket* sock, sf::Packet p, std::string* mssg, bool* exit) {
	std::string mssg_temp;
	while (*exit) {
		sock->receive(p);
		if (p.getDataSize() > 0) {
			p >> mssg_temp;
			//p << mssg_temp;
			p.clear();
			mssg->assign(mssg_temp);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}

void Client::connectClient(sf::TcpSocket* sock) {
	// Connection to the server
	sf::Socket::Status status = sock->connect("127.0.0.1", 5000, sf::seconds(5.f));
	if (status != sf::Socket::Done)
	{
		// If not connected sucessfully
		std::cout << "failed connection" << std::endl;
	}
}

void Client::ClientMain() 
{
	bool exit = true;

	// Client mode
	std::cout << "Client mode running" << std::endl;
	std::thread connect_client(&Client::connectClient, this, &socket);
	connect_client.join();
	//Connection to the server
	//sf::Socket::Status status = socket.connect("127.0.0.1", 5000, sf::seconds(5.f));
	//if (status != sf::Socket::Done)
	//{
	//	// If not connected sucessfully 
	//}
	int counter = 0;
	sf::Packet inPacket, outPacket;
	std::string sendMessage, rcvMessage;
	//outPacket << "hola";
	//socket.send(outPacket);
	//outPacket.clear();

	// Threads
	std::thread rcv_t(&Client::receive_and_return_cl, this, &socket, inPacket, &rcvMessage, &exit);
	rcv_t.detach();
	std::thread read_console_t(&Client::GetLineFromCin_t, this, &sendMessage, &exit);
	read_console_t.detach();

	while (true) {
		// Logic for receiving
		if (rcvMessage.size() > 0) {
			if (rcvMessage == "exit") {
				// Manages the desconection 
				break;
			}
			std::cout << rcvMessage << std::endl;;
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
				send_pkt(&socket, sendMessage);
				sendMessage.clear();
			}
		}
	}
	// When the application loop is broken, we have to release resources
	socket.disconnect();
}
