#include "ServerGame.h"
#include <iostream>
#include <fstream>
#include "Pieces.h"
void ServerGame::init()
{
    int b[64] = { -1,-2,-3,-4,-5,-3,-2,-1,
                    -6,-6,-6,-6,-6,-6,-6,-6,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0,
                    6, 6, 6, 6, 6, 6, 6, 6,
                    1, 2, 3, 4, 5, 3, 2, 1 };
    int spritepos[64] = { 0,1,2,3,4,5,6,7,
                                8,9,10,11,12,13,14,15,
                                64,64,64,64,64,64,64,64,
                                64,64,64,64,64,64,64,64,
                                64,64,64,64,64,64,64,64,
                                64,64,64,64,64,64,64,64,
                                48,49,50,51,52,53,54,55,
                                56,57,58,59,60,61,62,63 };
    for (int j = 0; j < 64; ++j) {
        board[j] = b[j];
        spritepositions[j] = spritepos[j];
    }
    done = false;
    checkMovement = false;
    win = false;
    resetTimer = false;
    isMove = false;
}

void ServerGame::updateboard(int n, int j) {
    int cc;
    cc = spritepositions[j];
    if (j != n) {
        int suppos = spritepositions[j];
        spritepositions[j] = spritepositions[n];
        spritepositions[n] = 64;
        if (board[j] == -5 || board[j] == 5) {
            // Game finished
            win = true;
        }
        if (j <= 63 & j >= 56 & board[n] == -6) {
            board[j] = -4;
        }
        else if (j >= 0 & j <= 7 & board[n] == 6) {
            board[j] = 4;
        }
        else {
            board[j] = board[n];
            board[n] = 0;
        }
        //n = j;
        std::cout << "NEXT TURN" << std::endl;
    }
}



void ServerGame::run(int n, int j)
{
    
    isMove = box.identifier(n, j, board[n], board);
    if (isMove) {
        resetTimer = true;
        updateboard(n, j);
        done = true;
        
    }
    else {
        resetTimer = true;
        done = true;
    }

}
