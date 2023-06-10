#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "ChessBoard.h"
#include <thread>


using namespace sf;
int main(){
	ChessBoard graphics;
	
	std::thread t_run(&ChessBoard::run, graphics);
	t_run.detach();
	ChessBoard graphics2;
	graphics2.run();
	return 0;
}