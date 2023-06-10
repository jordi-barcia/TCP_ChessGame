#pragma once
#include <SFML\Network.hpp>
#include <iostream>
#include <future>
#include <chrono>
#include <thread>
#include <mutex>

class Client
{

	std::mutex mtx;
public:
	void GetLineFromCin_t(std::string* mssg, bool* exit);
	void send_pkt(sf::TcpSocket* sock, std::string mssg);
	void receive_and_return_cl(sf::TcpSocket* sock, sf::Packet p, std::string* mssg, bool* exit);
	void connectClient(sf::TcpSocket* sock);
	void ClientMain();
};

