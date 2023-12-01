#ifndef GAME_H
#define GAME_H

// #include "../client2.h"

typedef struct 
{
	int board[12];
	int tmp[12];
	int challenger; // playerA
	int scoreA;
	int challenged; // playerB
	int scoreB;
	char currentPlayer;
	char active; // 0 for pending, 1 for playing
	char finished;
} Game;


char generateFirstPlayer();
char initGame(Game* g);
int convertGame(int userInput);
int nextArrayElem(int baseIndex);
int previousArrayElem(int baseIndex);
int canTake(int *array, int index, int userInput);
int playMove(const int baseArray[], int outArray[], const int userInput);
int playMoveAndTake(const int *baseArray, int *outArray, int userInput);
char nextPlayer(char player);
char empty(int board[12], char side);
int sumSide(int board[12], char side);
int bestMove(int board[12], char player);
char moveOkay(Game g, int move);
void show_board(const int board[]);
int getUserInput(Game g);
int playTheMove(Game* g, int computerMove);

#endif /* guard */
