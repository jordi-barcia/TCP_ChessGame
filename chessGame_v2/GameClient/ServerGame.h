#pragma once
#include <iostream>
#include <fstream>
#include "Pieces.h"
using namespace std;

class ServerGame
{
    int spritepositions[64] = { 0,1,2,3,4,5,6,7,
                                8,9,10,11,12,13,14,15,
                                64,64,64,64,64,64,64,64,
                                64,64,64,64,64,64,64,64,
                                64,64,64,64,64,64,64,64,
                                64,64,64,64,64,64,64,64,
                                48,49,50,51,52,53,54,55,
                                56,57,58,59,60,61,62,63 };
    int board[64] = { -1,-2,-3,-4,-5,-3,-2,-1,
                    -6,-6,-6,-6,-6,-6,-6,-6,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    6, 6, 6, 6, 6, 6, 6, 6,
                    1, 2, 3, 4, 5, 3, 2, 1 };
   
public:
    bool isMove;
    int gameID = 0;
    bool resetTimer = false;
	void run(int n, int j);
    bool done = false;
    bool checkMovement = false;
    bool win = false;
    Identity box;
    void updateboard(int n, int j);
    void init();
};

