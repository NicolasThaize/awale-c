
#include "game.h"

#ifdef TRACE
#define debug(expression) (printf("%s:%d -> " #expression "\n",__FILE__,__LINE__))
#define debugd(expression) (printf("%s:%d -> %d " #expression "\n",__FILE__,__LINE__,expression))
#define debugc(expression) (printf("%s:%d -> %c " #expression "\n",__FILE__,__LINE__,expression))
#else
#define debug(expression) ((void)0)
#define debugd(expression) ((void)0)
#define debugc(expression) ((void)0)
#endif



char init(int board[12]) {}


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
	g.tmp = playMoveAndTake(g.board, move);
	if ( empty(g.board,nextPlayer(g.player)) ) {
		if ( move == bestMove(g.board, g.player) ) {
			// OK (move that give the most)
			return 1;
		} else if ( bestMove(g.board, g.player) == -1 ) {
			// no moves that give pieces => player win
			return 2; // normally this case no append
		} else {
			// not allowed
			return 0;
		}
	} else {
		if ( empty(g.tmp,nextPlayer(g.player)) ) {
			// OK (could take everything so take nothing)
			g.tmp = playMove(g.board, move); // do not take
			return 1;
		} else {
			// OK
			return 1;
		}
	}
}


int main() {
	Game g;
	int move;
	init(g);
	while (!g.finished) {
		showBoard(g.board);
		printf("Player %d",g.player);
		scanf(" move : %d",&move);
		if (moveOkay(g,move)) {
			g.board = g.tmp;
			g.player = nextPlayer(g.player);
			if ( empty(g.board,nextPlayer(g.player)) && bestMove(g.board, g.player) == -1 ) // move that make you loose
			{
				g.finished = 1;
				printf("Player %d win !\n",g.player);
			}
		} else if ( moveOkay(g,move) == 1 ) {
			printf("Forced move : you have to use the move that give the most to the opponent\n");
		} else if ( moveOkay(g,move) == 2 ) {
			printf("It appear that you have win\n");
		} else {
			printf("There is an error\n");
		}
	}
}



