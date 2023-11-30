#include "game.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifdef TRACE
#define debug(expression) (printf("%s:%d -> " #expression "\n",__FILE__,__LINE__))
#define debugd(expression) (printf("%s:%d -> %d " #expression "\n",__FILE__,__LINE__,expression))
#define debugc(expression) (printf("%s:%d -> %c " #expression "\n",__FILE__,__LINE__,expression))
#else
#define debug(expression) ((void)0)
#define debugd(expression) ((void)0)
#define debugc(expression) ((void)0)
#endif



static char generateFirstPlayer() {
    srand(time(NULL)); 
    int r = rand() % 2;
    char currentJoueur[2] = {1, 2}; // {'A', 'B'}
    return currentJoueur[r];
}

static char init(Game* g) {
	for (int i=0; i<12; i++) {
		g->board[i] = 4;
	}
	g->currentPlayer = generateFirstPlayer();
	g->finished = 0;
	g->scoreA = 0;
	g->scoreB = 0;
	return 1; // everything ok
}

static int convert(int userInput) {
	debugd(userInput);
	int res;
	if ( userInput <= 12 && userInput >= 7 ) {
		res = 12 - userInput;
	} else if ( userInput >= 1 && userInput <= 6 ) {
		res = userInput + 5;
	}
	else {
		res = -1;
	}
	return res;
}

static int nextArrayElem(int baseIndex) {
    if (baseIndex == 0) {
        return 6;
    } else if (baseIndex == 11) {
        return 5;
    } else if (baseIndex < 6 && baseIndex > 0) {
        return baseIndex - 1;
    } else if (baseIndex < 12 && baseIndex > 5) {
        return baseIndex + 1;
    } else {
		debugd(baseIndex);
        return -1;
    }    
}

static int previousArrayElem(int baseIndex) {
    if (baseIndex == 6) {
        return 0;
    } else if (baseIndex == 5) {
        return 11;
    } else if (baseIndex < 6 && baseIndex > 0) {
        return baseIndex + 1;
    } else if (baseIndex < 12 && baseIndex > 5) {
        return baseIndex - 1;
    } else {
        return -1;
    }    
}

static int getPositionSide(int input) {
	if (input >= 0 && input <=5) {
		return 2;
	} else if (input >=6 && input <= 11) {
		return 1;
	} else {
		debug("PAS NORMAL getPositionSide, input ");
		return -1;
	}
	
}

static int canTake(int *array, int index, int userInput) {
	if (getPositionSide(index) == getPositionSide(userInput))
	{
		return -1;
	}
	

    if (array[index] == 2 || array[index] == 3) {
        const int value = array[index];
        array[index] = 0;
		debug("canTake:");
		debugd(index);
        return value;
    }
    return -1;
}

static int playMove(const int baseArray[], int outArray[], const int userInput) {
	assert(0<=userInput);
	assert(userInput<12);
	for (int i=0; i<12; i++) {
		outArray[i] = baseArray[i];
	}
	int current = userInput;
    int valueToAdd = baseArray[current];

    outArray[current] = 0;
    //printf("\n Value to add: %d \n", valueToAdd);

    for (int i = valueToAdd; i > 0; i--) {
        current = nextArrayElem(current);
		if (current != userInput) {
	        outArray[current]++;
			debugd(current);
			debugd(outArray[current]);
		} else {
			i++; // skip case
			debugd(i);
			debugd(current);
		}
    }

    //printf("\n New array : \n");
    // for(int i = 0; i < 12; i++) {
    //    debugd(outArray[i]);
    // }

    return current; // return the position of the last seed
}

static int playMoveAndTake(const int *baseArray, int *outArray, int userInput) {
	int current = playMove(baseArray,outArray,userInput);
	debug("current :");
	debugd(current);

	int recuperedSeeds = 0;
	int valueToGive = canTake(outArray, current, userInput);
    while (valueToGive != -1) {
        recuperedSeeds += valueToGive;
        current = previousArrayElem(current);
        valueToGive = canTake(outArray, current, userInput);
    }
    
    return recuperedSeeds;
}

static char nextPlayer(char player) {
	if ( player == 1 ) {
		return 2;
	} else if ( player == 2 ) {
		return 1;
	} else {
		debug("Error next player");
		return -1; // error
	}
}

static char empty(int board[12], char side) {
	if ( side == 1 ) {
		if ( ( 0 == board[6] ) &&
				( 0 == board[7] ) &&
				( 0 == board[8] ) &&
				( 0 == board[9] ) &&
				( 0 == board[10] ) &&
				( 0 == board[11] ) ) {
			return 1;
		} else {
			return 0;
		}
	} else if ( side == 2 ) {
		if ( ( 0 == board[0] ) &&
				( 0 == board[1] ) &&
				( 0 == board[2] ) &&
				( 0 == board[3] ) &&
				( 0 == board[4] ) &&
				( 0 == board[5] ) ) {
			return 1;
		} else {
			return 0;
		}
	} else {
		debug("error empty");
		return -1; // error
	}
}

static int sumSide(int board[12], char side) {
	int sum = 0;
	if ( side == 1 ) {
		for (int i=6; i<12; i++) {
			sum += board[i];
		}
	} else if ( side == 0 ) {
		for (int i=0; i<6; i++) {
			sum += board[i];
		}
	} else {
		sum = -1;
	}
	return sum;
}

static int bestMove(int board[12], char player) {
	int max = 0;
	int iMax = -1;
	int tmp[12];
	for (int i=0; i<12; i++) {
		tmp[i] = board[i];
	}
	for (int i=0; i<6; i++) {
		playMoveAndTake(board,tmp,i+(player-1)*6); // play move 0..5 if player == 1 and move 6..11 if player == 2
			show_board(board);
		if ( sumSide(board,nextPlayer(player)) > max ) {
			max = sumSide(board,nextPlayer(player));
			iMax = i+(player-1)*6;
		}
	}
	return iMax;
}

static char moveOkay(Game g, int move) {
	playMoveAndTake(g.board, g.tmp, move);
	// play move on board to tmp and take and put taking into score
	if ( empty(g.board,nextPlayer(g.currentPlayer)) == 1 ) {
		if ( move == bestMove(g.board, g.currentPlayer) ) {
			// OK (move that give the most)
			return 1;
		} else if ( bestMove(g.board, g.currentPlayer) == -1 ) {
			// no moves that give pieces => player win
			return 2; // normally this case no append
		} else {
			// not allowed
			return 0;
		}
	} else {
		if ( empty(g.tmp,nextPlayer(g.currentPlayer)) == 1 ) {
			// OK (could take everything so take nothing)
			playMove(g.board, g.tmp, move); // play move on board to tmp
			return 3;
		} else {
			// OK
			return 1;
		}
	}
}

static void show_board(const int board[]) {
	printf("\n");
	for (int i=12; i>9; i--) {
		printf("  %d",i);
	}
	for (int i=9; i>6; i--) {
		printf("  %d ",i);
	}
	printf("\n");
	for (int i=0; i<6; i++)
		printf(" ---");
	printf("\n");
	for (int i=0; i<6; i++) {
		if (board[i] > 9) {
			printf("|%d ",board[i]);
		} else {
			printf("| %d ",board[i]);
		}
	} printf("|\n");
	for (int i=0; i<6; i++)
		printf(" ---");
	printf("\n");
	for (int i=0; i<6; i++) {
		if (board[i] > 9) {
			printf("|%d ",board[i+6]);
		} else {
			printf("| %d ",board[i+6]);
		}
	} printf("|\n");
	for (int i=0; i<6; i++)
		printf(" ---");
	printf("\n");
	for (int i=0; i<6; i++) {
		printf("  %d ",i+1);
	}
	printf("\n\n");
}


int main() {
	debugd(nextArrayElem(5));
	Game g;
	int humanMove;
	int computerMove;
	init(&g);
	debug("Here");
	while ( g.finished == 0 ) {
		printf("-------------- TOUR 1 ----------------");
		show_board(g.board);
		humanMove = -1;
		debugd(g.currentPlayer);
		while ( g.currentPlayer == 1 && ( humanMove <= 0 || humanMove > 6 ) ) {
			printf("Player 1 : ");
			scanf("%d",&humanMove);
		}
		while ( g.currentPlayer == 2 && ( humanMove <= 6 || humanMove > 12 ) ) {
			printf("Player 2 : ");
			scanf("%d",&humanMove);
		}
	    computerMove = convert(humanMove);
		debugd(computerMove);

		if ( moveOkay(g,computerMove) == 1 ) {
			debug("improve score and apply move");
			printf("Move okay");
			if ( g.currentPlayer == 1 ) {
				g.scoreA += playMoveAndTake(g.board,g.board,computerMove);
			} else if ( g.currentPlayer == 2 ) {
				g.scoreB += playMoveAndTake(g.board,g.board,computerMove);
			} else {
				debug("current player pb");
			}
			debug("test win");
			if ( ( g.currentPlayer == 1 && g.scoreA > 24 ) || ( g.currentPlayer == 2 && g.scoreB > 24 ) ) {
				g.finished = 1;
				printf("Player %d win !\n",g.currentPlayer);
				break;
			}
			debug("test loose");
			if ( empty(g.board,nextPlayer(g.currentPlayer)) && bestMove(g.board, g.currentPlayer) == -1 ) {
				g.finished = 1;
				printf("Player %d win !\n",g.currentPlayer);
				break;
			}
			g.currentPlayer = nextPlayer(g.currentPlayer);
		} else if ( moveOkay(g,computerMove) == 3 ) {
			printf("You want to take everything from your opponent but you can't so you take nothing\n");
			playMove(g.board,g.board,computerMove);
			g.currentPlayer = nextPlayer(g.currentPlayer);
			g.finished = 1;
		} else if ( moveOkay(g,computerMove) == 2 ) {
			printf("It appear that you have win\n");
			g.finished = 1;
		} else {
			printf("There is an error\n");
			debugd(moveOkay(g,computerMove));
			g.finished = 1;
		}
	}
}




