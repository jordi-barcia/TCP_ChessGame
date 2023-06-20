#pragma once
#include <SFML\Network.hpp>
#include <iostream>
#include <future>
#include <chrono>
#include <thread>
#include <mutex>
#include "ChessBoard.h"

class Client
{
	std::mutex mtx;
	sf::TcpSocket socket;
	ChessBoard game;
	bool doneSent = false;
	bool receivedMovement = false;
	bool firstMessage = false;
	bool newGame = false;
	std::string temporalMessage;
public:
	void GetLineFromCin_t(std::string* mssg, bool* exit);
	void send_pkt(sf::TcpSocket* sock, std::string mssg); // Funcion que nos sirve para enviar mensajes al servidor. 
	void receive_and_return_cl(sf::TcpSocket* sock, sf::Packet p, std::string* mssg, bool* exit); // Funcion que nos permite recibir y devolver los mensajes al servidor. 
	void connectClient(sf::TcpSocket* sock); // Funcion que nos permite connectar el cliente con el servidor.
	void ClientMain(); // Main principal del cliente donde gestiona todas las funciones anteriores. 
};

