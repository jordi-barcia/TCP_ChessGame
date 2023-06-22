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
#include <stdlib.h>
#include "Timer.h"
#include "ServerGame.h"

class Server
{
	std::mutex mtx;
	std::string s_port;
	std::string temporalMessage;
	int ID = 0;
	std::vector<int>IDgames; // Vector de games para gestionar las partidas. 
	std::vector<bool>inGame;
	int count = 0;
	int createGames = 0;
	bool checkPositions = false;
	bool firstMessage = false;
	bool newGame = false;
	int temp_n = -1, temp_j = -1;
	int last_n = 0, last_j = 0;
	int check = 0;
	int temp = -1;
	int socketWinner = -1;
	int gameWinner = -1;
	ServerGame s_game;
	std::vector<ServerGame> s_games;

	//Timer
	Timer timer;
	std::vector<Timer> timers; // Timer para gestionar la desconexion de cada uno de los clientes. 
	int duration = 120;
	int inGameDuration = 60;
	int firstToPlay;

public:
	std::vector<sf::TcpSocket*> sockets; // Vector de sockets para gestionar los clientes connectados. 

	void GetLineFromCin_t(std::string* mssg, bool* exit);
	void receive_and_return(std::vector<sf::TcpSocket*>* sockets, std::string* mssg, unsigned int* port, bool* exit); // Funcion para recibir los paquetes de los clientes. 
	void send_pkt_sv(std::vector<sf::TcpSocket*>* sockets, std::string mssg, unsigned int* port); // Funcion que permite enviar el paquete de un cliente a todos los demas.
	void acceptSocket(sf::TcpListener* dispatcher, std::vector<sf::TcpSocket*>* sockets, bool* exit); // Accepta los sockets de los clientes. 
	void welcome(sf::TcpSocket* sock, std::string mssg); // Funcion que envia el paquete de welcome al cliente. 
	void packageControl(sf::TcpSocket* sock, std::string mssg); // Funcion para enviar un mensaje a un solo cliente. 
	void ServerMain(); // Main principal del Servidor donde gestiona todas las funciones anteriores. 
	void timerDisconnection(); // Funcion que nos permite hacer una cuenta atras para desconnectar cada uno de los clientes. 
	void winnerGame(int i, int j); // Recibimos quien ha ganado la partida y en que GAME y le enviamos un mensaje de VICTORIA/DERROTA a los jugadores y les preguntamos si quieren jugar otra partida
};

