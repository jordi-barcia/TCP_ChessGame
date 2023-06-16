#include "ServerGame.h"
#include "Pieces.h"
void ServerGame::run(int n, int j)
{
	Identity box;
	//Recibir envio cliente

	// New position
	isMove = box.identifier(n, j, board[n], board);
	if (isMove) {
		resetTimer = true;
		done = true;
	}
	else {
		done = true;
	}

}