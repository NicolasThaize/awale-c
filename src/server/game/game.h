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
char init(int board[12]);

char init(int board[12]);
char nextPlayer(char player);
char empty(int board[12], char side);
int sumSide(int board[12], char side);
int bestMove(int board[12], char player);
char moveOkay(Game g, int move);

#endif /* guard */
