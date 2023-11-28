#ifndef GAME_H
#define GAME_H


struct game
{
	int board[12];
	int tmp[12];
	char player;
};


char notEmpty(int board[12]);
char forcedMove(int board[12]);
char init(int board[12]);

#endif /* guard */
