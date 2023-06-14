#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>

using namespace sf;
using namespace std;

class ServerGame
{
	int turn = 1;
public:
	bool updateboard(int n, int j, sf::RectangleShape rectangle[64], sf::Sprite sprite[65]);
	void run();
};

