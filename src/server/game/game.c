#include "game.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef TRACE
#define debug(expression) (printf("%s:%d -> " #expression "\n",__FILE__,__LINE__))
#define debugd(expression) (printf("%s:%d -> %d " #expression "\n",__FILE__,__LINE__,expression))
#define debugc(expression) (printf("%s:%d -> %c " #expression "\n",__FILE__,__LINE__,expression))
#else
#define debug(expression) ((void)0)
#define debugd(expression) ((void)0)
#define debugc(expression) ((void)0)
#endif


char generateFirstPlayer() {
    srand(time(NULL)); 
    int r = rand() % 2;
    char currentJoueur[2] = {1, 2}; // {'A', 'B'}
    return currentJoueur[r];
}


char init(Game g) {
	for (int i=0; i<12; i++) {
		g.board[i] = 4;
	}
	g.currentPlayer = generateFirstPlayer();
	g.finished = 0;
	g.scoreA = 0;
	g.scoreB = 0;
}


static int convert(int userInput) {
   int res;
   if ( userInput < 13 && userInput > 6 ) {
      res = 12 - userInput;
   } else if ( userInput > 0 && userInput < 7 ) {
      res = userInput + 5;
   }
   else {
      res = -1;
   }
   return res;
}

int nextArrayElem(int baseIndex) {
    if (baseIndex == 0) {
        return 6;
    } else if (baseIndex == 11) {
        return 5;
    } else if (baseIndex < 6 && baseIndex > 0) {
        return baseIndex - 1;
    } else if (baseIndex < 12 && baseIndex > 5) {
        return baseIndex + 1;
    } else {
        return -1;
    }    
}

char playMove(const int *baseArray, int *outArray, int userInput) {
	int current = userInput;
    int valueToAdd = baseArray[current];

    outArray[current] = 0;
    //printf("\n Value to add: %d \n", valueToAdd);

    for (int i = valueToAdd; i > 0; i--) {
        int current = nextArrayElem(current);
		if (current != userInput) {
	        outArray[current]++;
		} else {
			i++; // skip case
		}
    }

    //printf("\n New array : \n");
    for(int i = 0; i < 12; i++) {
       debugd(outArray[i]);
    }

    return 1;
}


char nextPlayer(char player) {
	if ( player == 1 ) {
		return 0;
	} else if ( player == 2 ) {
		return 1;
	} else {
		return -1; // error
	}
}

char empty(int board[12], char side) {
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
		return -1; // error
	}
}

int sumSide(int board[12], char side) {
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

int bestMove(int board[12], char player) {
	int max = 0;
	int iMax = -1;
	for (int i=0; i<6; i++) {
		playMoveAndTake(board,i+(player-1)*6); // play move 0..5 if player == 1 and move 6..11 if player == 2
		if ( sumSide(board,nextPlayer(player)) > max ) {
			max = sumSide(board,nextPlayer(player));
			iMax = i+(player-1)*6;
		}
	}
	return iMax;
}


char moveOkay(Game g, int move) {
	int aScore;
	int bScore;
	playMoveAndTake(g.board, g.tmp, move, aScore, bScore);
	// play move on board to tmp and take and put taking into score
	if ( empty(g.board,nextPlayer(g.currentPlayer)) ) {
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
		if ( empty(g.tmp,nextPlayer(g.currentPlayer)) ) {
			// OK (could take everything so take nothing)
			playMove(g.board, g.tmp, move); // play move on board to tmp
			return 1;
		} else {
			// OK
			return 1;
		}
	}
}


int main() {
	Game g;
	int humanMove;
	int computerMove;
	init(g);
	while (!g.finished) {
		showBoard(g.board);
		printf("Player %d",g.currentPlayer);
		scanf(" move : %d",&humanMove);
	    int computerMove = convert(humanMove);
		if (moveOkay(g,computerMove)) {
			for (int i=0; i<12; i++) {
				g.board[i] = g.tmp[i];
			}
			if ( ( g.currentPlayer == 1 && g.scoreA > 24 ) || ( g.currentPlayer == 2 && g.scoreB > 24 ) ) {
				// move that make you win
				g.finished = 1;
				printf("Player %d win !\n",g.currentPlayer);
			}
			g.currentPlayer = nextPlayer(g.currentPlayer);
			if ( empty(g.board,nextPlayer(g.currentPlayer)) && bestMove(g.board, g.currentPlayer) == -1 ) {
				// move that make you loose
				g.finished = 1;
				printf("Player %d win !\n",g.currentPlayer);
			}
		} else if ( moveOkay(g,computerMove) == 1 ) {
			printf("Forced move : you have to use the move that give the most to the opponent\n");
		} else if ( moveOkay(g,computerMove) == 2 ) {
			printf("It appear that you have win\n");
		} else {
			printf("There is an error\n");
		}
	}
}




