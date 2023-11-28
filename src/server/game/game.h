#ifndef GAME_H
#define GAME_H

#include "../client2.h"

typedef struct 
{
	int board[12];
	int tmp[12];
	Client* playerA;
	Client* playerB;
	char currentPlayer;
} Game;


char notEmpty(int board[12]);
char forcedMove(int board[12]);
char generateFirstPlayer();
Game* init();

#endif /* guard */
