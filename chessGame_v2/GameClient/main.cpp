#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "ChessBoard.h"
#include <thread>
#include "Server.h"
#include "Client.h"

using namespace sf;
int main(){
	
	ChessBoard graphics;
	Server server;
	Client client;
	int number = 0;
	
	std::cout << "Select Server (1) or Client (2):" << std::endl;
	std::cin >> number;
	if (number == 1)
	{		
		server.ServerMain();
	}
	else if (number == 2)
	{
		client.ClientMain();
	}
	else
	{
		std::cout << "Select Server (1) or Client (2):" << std::endl;
		std::cin >> number;
	}

	/*std::thread t_run(&ChessBoard::run, graphics);
	t_run.detach();
	ChessBoard graphics2;
	graphics2.run();*/
	

	return 0;
}