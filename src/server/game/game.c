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


char notEmpty(int board[12]) {}
char forcedMove(int board[12]) {}

char generateFirstPlayer() {
    srand(time(NULL)); 
    int r = rand() % 2;
    char currentJoueur[2] = {'A', 'B'};
    return currentJoueur[r];
}

Game* init(Client* playerA, Client* playerB) {
    Game g = {{4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4}, {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4}, playerA, playerB, generateFirstPlayer()};
    return &g;
}



