#include "ServerGame.h"
#include "Pieces.h"

bool ServerGame::updateboard(int n, int j) {
    int cc;
    bool game_finished;
    std::cout << "NEXT TURN" << std::endl;
    cc = spritepositions[j];
    if (j != n) {
        int suppos = spritepositions[j];
        spritepositions[j] = spritepositions[n];
        spritepositions[n] = 64;
        if (board[j] == -5 || board[j] == 5) {
            // Game finished
            return true;
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
        n = j;
    }
    return false; // Game not finished
}
void ServerGame::run(int n, int j)
{
	Identity box;

	// New position
	isMove = box.identifier(n, j, board[n], board);
	if (isMove) {
		resetTimer = true;
        updateboard(n, j);
		done = true;
	}
	else {
		done = true;
	}

}