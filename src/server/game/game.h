#ifndef GAME_H
#define GAME_H

// #include "../client2.h"

typedef struct 
{
	int board[12];
	int tmp[12];
	// Client* playerA;
	int scoreA;
	// Client* playerB;
	int scoreB;
	char currentPlayer;
	char finished;
} Game;


static char generateFirstPlayer();
static char init();
static int convert(int userInput);
static int nextArrayElem(int baseIndex);
static int previousArrayElem(int baseIndex);
static int canTake(int *array, int index);
static int playMove(const int baseArray[], int outArray[], const int userInput);
static int playMoveAndTake(const int *baseArray, int *outArray, int userInput);
static char nextPlayer(char player);
static char empty(int board[12], char side);
static int sumSide(int board[12], char side);
static int bestMove(int board[12], char player);
static char moveOkay(Game g, int move);
static void show_board(const int board[]);


#endif /* guard */
