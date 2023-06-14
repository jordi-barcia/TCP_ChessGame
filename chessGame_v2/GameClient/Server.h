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
#include "ServerGame.h"

class Server
{
	std::mutex mtx;
	std::string s_port;
	std::vector<ChessBoard>games;
	int count = 0;
	int createGames = 0;


	//Timer
	Timer timer;
	std::vector<Timer> timers;
	int duration = 120;
	int inGameDuration = 60;
	int firstToPlay;

public:
	std::vector<sf::TcpSocket*> sockets;
	void GetLineFromCin_t(std::string* mssg, bool* exit);
	void receive_and_return(std::vector<sf::TcpSocket*>* sockets, /*sf::Packet p,*/ std::string* mssg, unsigned int* port, bool* exit);
	void send_pkt_sv(std::vector<sf::TcpSocket*>* sockets, std::string mssg, unsigned int* port);
	void acceptSocket(sf::TcpListener* dispatcher, std::vector<sf::TcpSocket*>* sockets, bool* exit);
	void welcome(sf::TcpSocket* sock, std::string mssg);
	void packageControl(sf::TcpSocket* sock, std::string mssg);
	void ServerMain();
	void timerDisconnection();
};

