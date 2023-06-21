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
			inGame.push_back(false);
			std::cout << timers.size() << std::endl;
			std::cout << inGame.size() << std::endl;

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
			if (timers[i].temp <= 0) {
				mtx.lock();
				if (createGames >= 1) { //Significa que hay games creados
					std::cout << "Cliente " << sockets[i]->getRemotePort() << " ha perdido por inactividad" << std::endl;
					packageControl(sockets[i], "HAS PERDIDO!");
					if (i % 2 == 0) { //Ha perdido un cliente par, por lo tanto, el primero de los 2 jugadores
						std::cout << "Cliente " << sockets[i + 1]->getRemotePort() << " has ganado!" << std::endl;
						packageControl(sockets[i + 1], "HAS GANADO!");
						std::this_thread::sleep_for(std::chrono::milliseconds(50));
						packageControl(sockets[i + 1], "Quieres jugar otra partida? Si/No");
						newGame = true;
						timers[i + 1].init(duration); // Reiniciamos el timer de desconexion de los clientes
						IDgames.erase(IDgames.begin() + i + 1); // Borramos las ID's del GAME
						IDgames.erase(IDgames.begin() + i);
					}
					else {
						std::cout << "Cliente " << sockets[i - 1]->getRemotePort() << " has ganado!" << std::endl;
						packageControl(sockets[i - 1], "HAS GANADO!");
						std::this_thread::sleep_for(std::chrono::milliseconds(50));
						packageControl(sockets[i - 1], "Quieres jugar otra partida? Si/No");
						newGame = true;
						timers[i - 1].init(duration); // Reiniciamos el timer de desconexion de los clientes
						IDgames.erase(IDgames.begin() + i); // Borramos las ID's del GAME
						IDgames.erase(IDgames.begin() + i - 1);

					}
					createGames--;
					std::this_thread::sleep_for(std::chrono::milliseconds(30));	
				}
				// Desconectamos al cliente inactivo.
				packageControl(sockets[i], "exit");
				sockets[i]->disconnect();
				delete sockets[i];
				sockets.erase(sockets.begin() + i);
				timers.erase(timers.begin() + i);
				inGame.erase(inGame.begin() + i);
				if (s_games.size() != 0) {
					if (i < s_games.size()) {
						s_games.erase(s_games.begin() + i);
					}
					else {
						s_games.erase(s_games.begin() + i - 1);
					}
				}
				std::cout << "Client disconnected. Total clients: " << sockets.size() << std::endl;
				count--;
				mtx.unlock();
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void Server::winnerGame(int i, int j) // Recibimos quien ha ganado la partida y en que GAME y le enviamos un mensaje de VICTORIA/DERROTA a los jugadores y les preguntamos si quieren jugar otra partida
{
	if (socketWinner != -1 && !s_games[j].done) {
		std::this_thread::sleep_for(std::chrono::milliseconds(150));
		mtx.lock();
		std::cout << "Cliente " << sockets[i]->getRemotePort() << " has ganado!" << std::endl;
		packageControl(sockets[i], "HAS GANADO!");
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		packageControl(sockets[i], "Quieres jugar otra partida? Si/No");
		timers[i].init(duration); // Reiniciamos el timer de desconexion de los clientes
		if (i % 2 == 0) { //Ha ganado un cliente par, por lo tanto, el primero de los 2 jugadores
			std::cout << "Cliente " << sockets[i + 1]->getRemotePort() << " has perdido!" << std::endl;
			packageControl(sockets[i + 1], "HAS PERDIDO!");
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			packageControl(sockets[i + 1], "Quieres jugar otra partida? Si/No");
			timers[i + 1].init(duration); 
			IDgames.erase(IDgames.begin() + i + 1); // Borramos las ID's de GAME
			IDgames.erase(IDgames.begin() + i);
		}
		else {
			std::cout << "Cliente " << sockets[i - 1]->getRemotePort() << " has perdido!" << std::endl;
			packageControl(sockets[i - 1], "HAS PERDIDO!");
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			packageControl(sockets[i - 1], "Quieres jugar otra partida? Si/No");
			timers[i - 1].init(duration);
			IDgames.erase(IDgames.begin() + i); // Borramos las ID's de GAME
			IDgames.erase(IDgames.begin() + i - 1);

		}
		createGames--;
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
		// Borramos el juego del vector de GAMES
		if (i < s_games.size()) {
			s_games.erase(s_games.begin() + i); 
			socketWinner = -1;
		}
		else {
			s_games.erase(s_games.begin() + i - 1);
			socketWinner = -1;
		}
		count--;
		newGame = true;
		mtx.unlock();
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

	srand(time(NULL));

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

		timerDisconnection(); // Se encarga del calculo para desconectar a los clientes inactivos
		winnerGame(socketWinner, gameWinner); //Se encarga de dar la victoria

		// Logic for receiving
		if (rcvMessage.size() > 0) 
		{
			if (rcvMessage == s_port + ":CheckPosition") { // Si el servidor recibe el primer mensaje del cliente, preparamos al servidor para recibir el movimiento
				checkPositions = true;
				rcvMessage.clear();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			if (rcvMessage == s_port + ":Si" && newGame) { // Si recibimos un "Si" le ponemos que no esta en partida y a la espera de un rival. 
				rcvMessage.clear();
				if (count == 1) {
					count++;
				}
				if (count < 1) {
					count = 1;
				}
				for (int i = 0; i < sockets.size(); i++)
				{
					if (sockets[i]->getRemotePort() == port)
					{
						// Cliente encontrado, ponemos que esta buscando partida otra vez
						std::this_thread::sleep_for(std::chrono::milliseconds(10));
						inGame[i] = false;
						packageControl(sockets[i], "Waiting for a new rival...");
					}
				}

				std::cout << count << std::endl;
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
						timers.erase(timers.begin() + i);
						inGame.erase(inGame.begin() + i);
						std::cout << "Client disconnected. Total clients: " << sockets.size() << std::endl;
						count--;
					}
				}
			}
			else if(!checkPositions && socketWinner == -1) {
				std::cout << rcvMessage << std::endl;
				sendMessage = rcvMessage;
				rcvMessage.clear();
			}	
			if (checkPositions) { // Recibimos el movimiento del cliente por partes. 
				if (!firstMessage && rcvMessage != s_port + ":Enviados") { // Recibimos la coordenada "n" y la guardamos en una variable temporal
					std::cout << rcvMessage << std::endl;
					temp_n = stoi(temporalMessage);
					firstMessage = true;
					rcvMessage.clear();
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
				if (firstMessage && rcvMessage != s_port + ":Enviados") { // Recibimos la coordenada "j" y la guardamos en una variable temporal
					std::cout << rcvMessage << std::endl;
					temp_j = stoi(temporalMessage);
					rcvMessage.clear();
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
				if (rcvMessage == s_port + ":Enviados") { // Una vez recibidas las coordenadas si el cliente nos confirma que ha enviado todas las coordenadas, buscamos en que game esta ese cliente y comprobamos su movimiento
					std::cout << "ENVIADO" << std::endl;
					for (int i = 0; i < sockets.size(); i++)
					{
						if (sockets[i]->getRemotePort() == port)
						{
							for (int j = 0; j < s_games.size(); j++) {

								if (IDgames[i] == s_games[j].gameID) { // Comprobamos que la ID del jugador sea la misma ID del GAME
									s_games[j].run(temp_n, temp_j);
								}

								if (s_games[j].win) { // Si el jugador mata al rei contrario guardamos las variables de que jugador ha ganado y en que partida para enviar el mensaje al ganador
									socketWinner = i;
									gameWinner = j;
									std::this_thread::sleep_for(std::chrono::milliseconds(20));
								}
							}
						}
					}
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

		if (count == 2) // Si es "2" quiere decir que dos personas estan buscando partida
		{
			int temp1 = -1, temp2 = -1;
			for (int i = 0; i < sockets.size(); i++) { // Buscamos en el vector de sockets que 2 personas estan buscando partida
				if (!inGame[i] && temp1 == -1) {
					temp1 = i;
					inGame[i] = true;
				}
				if (!inGame[i] && temp1 != -1) {
					temp2 = i;
					inGame[i] = true;
				}
			}
			
			//Creación de los games para los 2 clientes
			int random = rand() % 2; //Valor random de 0-1 para escoger quien juega las fichas blancas o negras
			std::string s_random = std::to_string(random);
			std::cout << "rand1 " << s_random  << std::endl;
			packageControl(sockets[temp1], s_random);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			timers[temp1].init(inGameDuration); //Reseteamos el temporizador del jugador y le ponemos el tiempo de desconexión de la partida
			packageControl(sockets[temp1], "Game");
			IDgames.push_back(ID); //Guardamos la ID del game en el vector de IDgames
			if (s_random == "0") {
				packageControl(sockets[temp2], "1");
			}
			else {
				packageControl(sockets[temp2], "0");
			}
			timers[temp2].init(inGameDuration); //Reseteamos el temporizador del jugador y le ponemos el tiempo de desconexión de la partida
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			packageControl(sockets[temp2], "Game");
			IDgames.push_back(ID); //Guardamos la ID del game en el vector de IDgames
			s_game.init();
			s_games.push_back(s_game);
			s_games[createGames].init();
			s_games[createGames].gameID = ID;
			ID++;
			createGames++;
			count = 0;
			newGame = false;

		}
		
		for (int k = 0; k < s_games.size(); k++) { // Buscamos en que GAME se tiene que enviar la validacion del movimiento
			if (s_games[k].isMove && s_games[k].done) {
				std::cout << "ENTRO ENVIO MOVIMIENTO" << std::endl;
				std::cout << sockets.size() << std::endl;
				for (int i = 0; i < sockets.size(); i++)
				{
					std::cout << i << std::endl;
					if (sockets[i]->getRemotePort() == port && temp == -1)
					{
						temp = i;
						packageControl(sockets[i], "Movimiento Correcto");
						timers[i].init(inGameDuration);

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
				s_games[k].resetTimer = false;
				s_games[k].done = false;
				temp = -1;
				std::cout << "MOVIMIENTOS ENVIADOS" << std::endl;
			}
			if (!s_games[k].isMove && s_games[k].done) {
				for (int i = 0; i < sockets.size(); i++)
				{
					if (sockets[i]->getRemotePort() == port && temp == -1)
					{
						temp = i;
						packageControl(sockets[i], "Movimiento Incorrecto");
					}
				}
				s_games[k].resetTimer = false;
				s_games[k].done = false;
				temp = -1;
			}
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
			timers.erase(timers.begin() + i);
			inGame.erase(inGame.begin() + i);
			IDgames.erase(IDgames.begin() + i);
			if (s_games.size() != 0) {
				s_games.pop_back();
			}
		}
		if (sockets.size() == 0) {
			break;
		}
	}
	mtx.unlock();
}
