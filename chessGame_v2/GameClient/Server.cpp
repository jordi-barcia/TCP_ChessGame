#include "Server.h"

void Server::GetLineFromCin_t(std::string* mssg, bool* exit) {
	while (*exit) {
		std::string line;
		std::getline(std::cin, line);
		mssg->assign(line);
	}
}

void Server::receive_and_return(std::vector<sf::TcpSocket*>* sockets, std::string* mssg, unsigned int* port, bool* exit) { // Recibe los paquetes de los clientes. 
	std::string mssg_temp;
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
				*port = sock->getRemotePort();
				s_port = std::to_string(*port);
				temporalMessage = mssg_temp;
				mssg_temp = s_port + ":" + mssg_temp;
				mssg->assign(mssg_temp);
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
		mtx.unlock();
	}
}

void Server::send_pkt_sv(std::vector<sf::TcpSocket*>* sockets, std::string mssg, unsigned int* port) { // Envia un mensaje a todos lo clientes excepto al que ha enviado el mensaje.
	sf::Packet p;
	p << mssg;
	mtx.lock();
	int i = 0;
	for (sf::TcpSocket* sock : *sockets) {

		if (sock->getRemotePort() != *port) {
			sf::Socket::Status status = sock->send(p);
		}
		else {
			timers[i].init(duration);
		}
		i++;
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
			count++;
			mtx.unlock();
			std::cout << "Acepta conexion" << std::endl;
			std::cout << sockets->size() << std::endl;
			port = sock->getRemotePort();
			std::string s_port = std::to_string(port);
			std::string s = "Connected " + s_port + " waiting for opponent";
			welcome(sock, s);
			timer.init(duration);
			timers.push_back(timer);
			std::cout << timers.size() << std::endl;
		}
	}
}

void Server::welcome(sf::TcpSocket* sock, std::string mssg) {
	sf::Packet p;
	p << mssg;
	sf::Socket::Status status = sock->send(p);
}

void Server::packageControl(sf::TcpSocket* sock, std::string mssg) // Envia un paquete a un cliente. 
{
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

void Server::timerDisconnection() {
	if (timers.size() > 0) {
		for (int i = 0; i < timers.size(); i++) {
			timers[i].update();
			//std::cout << timers[i].temp << std::endl;
			if (timers[i].temp <= 0) {
				mtx.lock();
				if (createGames > 1) { //Significa que hay games creados
					std::cout << "Cliente " << sockets[i]->getRemotePort() << " ha perdido por inactividad" << std::endl;
					if (i % 2 == 0) { //Ha perdido un cliente par, por lo tanto, el primero de los 2 jugadores
						std::cout << "Cliente " << sockets[i + 1]->getRemotePort() << " has ganado!" << std::endl;
					}
					else {
						std::cout << "Cliente " << sockets[i - 1]->getRemotePort() << " has ganado!" << std::endl;
					}
					createGames -= 2;
					std::this_thread::sleep_for(std::chrono::milliseconds(30));	
				}
				packageControl(sockets[i], "exit");
				sockets[i]->disconnect();
				delete sockets[i];
				sockets.erase(sockets.begin() + i);
				timers.erase(timers.begin() + i);
				std::cout << "Client disconnected. Total clients: " << sockets.size() << std::endl;
				count--;
				mtx.unlock();
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
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
	std::thread rcv_t(&Server::receive_and_return, this, &sockets, &rcvMessage, &port, &exit);
	rcv_t.detach();
	std::thread read_console_t(&Server::GetLineFromCin_t, this, &sendMessage, &exit);
	read_console_t.detach();

	// Application loop
	while (true) {

		timerDisconnection(); // Se encarga del calculo para desconectar a los clientes.

		// Logic for receiving
		if (rcvMessage.size() > 0) 
		{
			if (rcvMessage == s_port + ":CheckPosition") {
				checkPositions = true;
				rcvMessage.clear();
				std::cout << "AAAAAAAAAAAAA" << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			
			//Gestionar la desconexion
			if (rcvMessage == s_port + ":exit" )
			{
				for (int i = 0; i < sockets.size(); i++)
				{
					if (sockets[i]->getRemotePort() == port)
					{
						// Cliente encontrado, desconectar y eliminar el socket
						packageControl(sockets[i], "exit");
						sockets[i]->disconnect();
						delete sockets[i];
						sockets.erase(sockets.begin() + i);
						std::cout << "Client disconnected. Total clients: " << sockets.size() << std::endl;
						count--;
					}
				}
			}
			else if(!checkPositions) {
				std::cout << rcvMessage << std::endl;
				sendMessage = rcvMessage;
				rcvMessage.clear();
			}	
			if (checkPositions) {
				if (!firstMessage && rcvMessage != s_port + ":Enviados") {
					std::cout << rcvMessage << std::endl;
					temp_n = stoi(temporalMessage);
					firstMessage = true;
					rcvMessage.clear();
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
				if (firstMessage && rcvMessage != s_port + ":Enviados") {
					std::cout << rcvMessage << std::endl;
					temp_j = stoi(temporalMessage);
					rcvMessage.clear();
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
				if (rcvMessage == s_port + ":Enviados") {
					std::cout << "ENVIADOOO" << std::endl;
					s_game.run(temp_n, temp_j);
					std::cout << "ENVIADOOO222222" << std::endl;
					firstMessage = false;
					checkPositions = false;
					rcvMessage.clear();
				}
			}
		}

		// Logic for sending
		if (sendMessage.size() > 0) {
			if (sendMessage == "exit") {
				// Desconnection
				exit = false;
				sendMessage.clear();
				break;
			}
			else {
				send_pkt_sv(&sockets, sendMessage, &port);
				sendMessage.clear();
			}
		}

		if (count == 2)
		{
			//Creación de los games para los 2 clientes
			int random = rand() % 2; //Valor random de 0-1 para escoger quien juega las fichas blancas o negras
			std::string s_random = std::to_string(random);
			packageControl(sockets[createGames], s_random);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			timers[createGames].init(inGameDuration); //Reseteamos el temporizador del jugador y le ponemos el tiempo de desconexión de la partida
			packageControl(sockets[createGames], "Game");
			IDgames.push_back(ID); //Guardamos la ID del game en el vector de IDgames
			createGames++;

			if (s_random == "0") {
				packageControl(sockets[createGames], "1");
			}
			else {
				packageControl(sockets[createGames], "0");
			}
			timers[createGames].init(inGameDuration); //Reseteamos el temporizador del jugador y le ponemos el tiempo de desconexión de la partida
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			packageControl(sockets[createGames], "Game");
			IDgames.push_back(ID); //Guardamos la ID del game en el vector de IDgames
			createGames++;
			ID++;
			count = 0;
		}
		

		if (s_game.resetTimer && s_game.done) {
			std::cout << "ENTRO ENVIO MOVIMIENTO" << std::endl;
			std::cout << sockets.size() << std::endl;
			for (int i = 0; i < sockets.size(); i++)
			{
				std::cout << i << std::endl;
				if (sockets[i]->getRemotePort() == port && temp == -1)
				{
					std::cout << "ppppppppppp" << std::endl;
					temp = i;
					packageControl(sockets[i], "Movimiento Correcto");
					std::cout << "ttttttttt" << std::endl;
					timers[i].init(inGameDuration);
				
					//std::this_thread::sleep_for(std::chrono::milliseconds(100));
					//
					//std::string s_n = std::to_string(temp_n);
					//packageControl(sockets[i], s_n);
					//std::this_thread::sleep_for(std::chrono::milliseconds(100));
					//
					//std::string s_j = std::to_string(temp_j);
					//packageControl(sockets[i], s_j);
					//std::this_thread::sleep_for(std::chrono::milliseconds(110));
					//std::cout << s_n + " : " + s_j << std::endl;
					for (int j = 0; j < sockets.size(); j++)
					{
						if (temp != j) {
							if (IDgames[j] == IDgames[temp]) {
								packageControl(sockets[j], "Movimiento Contrario Correcto");

								std::this_thread::sleep_for(std::chrono::milliseconds(100));

								std::string s_n = std::to_string(temp_n);
								packageControl(sockets[j], s_n);
								std::this_thread::sleep_for(std::chrono::milliseconds(100));

								std::string s_j = std::to_string(temp_j);
								packageControl(sockets[j], s_j);
								std::this_thread::sleep_for(std::chrono::milliseconds(110));
								std::cout << s_n + " : " + s_j << std::endl;
							}
						}
					}
				}
			}
			s_game.resetTimer = false;
			s_game.done = false;
			temp = -1;
			std::cout << "MOVIMIENTOS ENVIADOS" << std::endl;
		}
		if (!s_game.resetTimer && s_game.done) {
			for (int i = 0; i < sockets.size(); i++)
			{
				if (sockets[i]->getRemotePort() == port && temp == -1)
				{
					temp = i;
					packageControl(sockets[i], "Movimiento Incorrecto");
				}
				for (int j = 0; j < sockets.size(); j++)
				{
					if (temp != j) {
						if (IDgames[j] == IDgames[temp]) {
							packageControl(sockets[i], "Movimiento Incorrecto");
						}
					}
				}
			}
			s_game.resetTimer = false;
			s_game.done = false;
			temp = -1;
		}

	}


	// When the application loop is broken, we have to release resources.
	dispatcher.close();
	mtx.lock();
	while (true) {
		for (int i = 0; i < sockets.size(); i++) {
			packageControl(sockets[i], "exit");
			sockets[i]->disconnect();
			delete sockets[i];
			sockets.erase(sockets.begin() + i);
		}
		if (sockets.size() == 0) {
			break;
		}
	}
	mtx.unlock();
}
