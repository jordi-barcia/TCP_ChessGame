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
	p.clear();
}

void Client::receive_and_return_cl(sf::TcpSocket* sock, sf::Packet p, std::string* mssg, bool* exit) {
	std::string mssg_temp;
	while (*exit) {
		sock->receive(p);
		if (p.getDataSize() > 0) {
			p >> mssg_temp;
			temporalMessage = mssg_temp;
			p.clear();
			mssg->assign(mssg_temp);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}

void Client::connectClient(sf::TcpSocket* sock)
{
	// Connection to the server
	sf::Socket::Status status = sock->connect("127.0.0.1", 5000, sf::seconds(5.f)); //Conectamos el cliente al servidor. IP: 127.0.0.1 . Puerto: 5000.
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
	std::thread connect_client(&Client::connectClient, this, &socket); // Iniciamos la conexion del cliente. 
	connect_client.join();

	int counter = 0;
	sf::Packet inPacket, outPacket;
	std::string sendMessage, rcvMessage;

	// Threads
	std::thread rcv_t(&Client::receive_and_return_cl, this, &socket, inPacket, &rcvMessage, &exit); // Ponemos la funcion de receive para poder interpretar los mensajes que llegan del servidor.
	rcv_t.detach();
	std::thread read_console_t(&Client::GetLineFromCin_t, this, &sendMessage, &exit);
	read_console_t.detach();

	while (true) {
		// Logic for receiving
		if (rcvMessage.size() > 0) {
			if (rcvMessage == "exit") {
				// Gestionar la desconexión
				exit = false;
				break;
			}
			if (rcvMessage == "Game") {
				// Abrir el juego por cada cliente
				unsigned int port;
				port = socket.getLocalPort();
				game.ID = port;
				std::string s_port = std::to_string(game.ID);
				std::cout << s_port << std::endl; // SUBJETIVO A SER BORRADO
				std::thread game(&ChessBoard::run, &game);
				game.detach();
			}
			if (rcvMessage == "1") { // Si el cliente es "1" sera las piezas BLANCAS.
				game.firstToPlay = 1;
				std::cout << game.firstToPlay << std::endl;
			}
			else if (rcvMessage == "0") { // Si el cliente es "0" sera las piezas NEGRAS.
				game.firstToPlay = 0;
				std::cout << game.firstToPlay << std::endl;
			}
			else if (rcvMessage != "Game") {
				std::cout << rcvMessage << std::endl;
			}
			if (rcvMessage == "Movimiento Contrario Correcto") {
				receivedMovement = true;
				rcvMessage.clear();
				std::this_thread::sleep_for(std::chrono::milliseconds(115));
			}
			if (rcvMessage == "Movimiento Correcto") {
				game.isMove = true;
				game.received = true;
				doneSent = false;
				game.sent = false;
				rcvMessage.clear();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			if (rcvMessage == "Movimiento Incorrecto") {
				game.isMove = false;
				game.received = false;
				doneSent = false;
				game.sent = false;
			}
			if (receivedMovement) {

				if (!firstMessage && rcvMessage != "Enviados") {
					std::cout << temporalMessage << std::endl;
					game.m = stoi(temporalMessage);
					firstMessage = true;
					rcvMessage.clear();
					std::this_thread::sleep_for(std::chrono::milliseconds(105));
				}
				if (firstMessage && rcvMessage != "Enviados") {
					std::cout << temporalMessage << std::endl;
					game.z = stoi(temporalMessage);
					rcvMessage.clear();
					std::this_thread::sleep_for(std::chrono::milliseconds(105));
				}
				if (rcvMessage =="Enviados") {
					std::cout << temporalMessage << std::endl;
					rcvMessage.clear();
					std::this_thread::sleep_for(std::chrono::milliseconds(105));
					
				}
				firstMessage = false;
				receivedMovement = false;
				game.hasMoved = true;
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
				game.cap = 1;
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
				game.isMove = true;
				game.received = true;
				doneSent = false;
				game.sent = false;
				
			}
			rcvMessage.clear();
		}

		// Logic for sending
		if (game.firstToPlay == -1) {
			if (sendMessage.size() > 0) {
				if (sendMessage == "exit") {
					//Desconexión
					send_pkt(&socket, sendMessage);
					sendMessage.clear();
				}
				else {
					send_pkt(&socket, sendMessage);
					sendMessage.clear();
				}
			}
		}

		if (game.sent && !doneSent) {
			sendMessage = "CheckPosition";
			send_pkt(&socket, sendMessage);
			sendMessage.clear();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			std::string s_n = std::to_string(game.n);
			send_pkt(&socket, s_n);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			std::string s_j = std::to_string(game.z);
			send_pkt(&socket, s_j);
			std::this_thread::sleep_for(std::chrono::milliseconds(105));
			std::cout << s_n + " : " + s_j << std::endl;

			sendMessage = "Enviados";
			send_pkt(&socket, sendMessage);
			sendMessage.clear();
			doneSent = true;
		}
	}

	// When the application loop is broken, we have to release resourcesº
	//socket.disconnect();
}


