#pragma once
#include <SFML\Network.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <future>
#include <chrono>
#include <thread>
#include <mutex>
#include "ChessBoard.h"
#include <time.h>
#include <stdio.h>
#include "Timer.h"

class Server
{
	std::mutex mtx;
	std::string s_port;
	std::vector<ChessBoard>games;
	int count = 0;
	
	//Timer
	Timer timer;
	std::vector<Timer> timers;
	bool startCooldown = false;
	int duration = 5;
	int inGameDuration = 60;

public:
	std::vector<sf::TcpSocket*> sockets;
	void GetLineFromCin_t(std::string* mssg, bool* exit);
	void receive_and_return(std::vector<sf::TcpSocket*>* sockets, /*sf::Packet p,*/ std::string* mssg, unsigned int* port, bool* exit);
	void send_pkt_sv(std::vector<sf::TcpSocket*>* sockets, std::string mssg, unsigned int* port);
	void acceptSocket(sf::TcpListener* dispatcher, std::vector<sf::TcpSocket*>* sockets, bool* exit);
	void welcome(sf::TcpSocket* sock, std::string mssg);
	void disconnect(sf::TcpSocket* sock, std::string mssg);
	void ServerMain();
};

