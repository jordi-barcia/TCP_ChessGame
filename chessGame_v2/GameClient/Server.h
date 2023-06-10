#pragma once
#include <SFML\Network.hpp>
#include <iostream> 
#include <future>
#include <chrono>
#include <thread>
#include <mutex>
class Server
{
	Server();

	std::mutex mtx;
public:
	void GetLineFromCin_t(std::string* mssg, bool* exit);
	void receive_and_return(std::vector<sf::TcpSocket*>* sockets, /*sf::Packet p,*/ std::string* mssg, unsigned int* port, bool* exit);
	void send_pkt_sv(std::vector<sf::TcpSocket*>* sockets, std::string mssg, unsigned int* port);
	void acceptSocket(sf::TcpListener* dispatcher, std::vector<sf::TcpSocket*>* sockets, bool* exit);
	void welcome(sf::TcpSocket* sock, std::string mssg);
	void ServerMain();
};

