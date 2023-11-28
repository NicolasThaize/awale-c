#ifndef GAME_H
#define GAME_H


struct game
{
	int board[12];
	int tmp[12];
	char player;
};


char init(int board[12]);
char nextPlayer(char player);
char empty(int board[12], char side);
int sumSide(int board[12], char side);
int bestMove(int board[12], char player);
char moveOkay(Game g, int move);

#endif /* guard */
