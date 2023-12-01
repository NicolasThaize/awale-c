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


static char generateFirstPlayer();
static char initGame(Game* g);
static int convertGame(int userInput);
static int nextArrayElem(int baseIndex);
static int previousArrayElem(int baseIndex);
static int canTake(int *array, int index, int userInput);
static int playMove(const int baseArray[], int outArray[], const int userInput);
static int playMoveAndTake(const int *baseArray, int *outArray, int userInput);
static char nextPlayer(char player);
static char empty(int board[12], char side);
static int sumSide(int board[12], char side);
static int bestMove(int board[12], char player);
static char moveOkay(Game g, int move);
static void show_board(const int board[]);
static int getUserInput(Game g);
static int playTheMove(Game* g, int computerMove);

#endif /* guard */
