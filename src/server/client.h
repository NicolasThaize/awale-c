#ifndef CLIENT_H
#define CLIENT_H

#define BUF_SIZE    1024
#define SMALL_SIZE	32

typedef int SOCKET;

typedef struct {
   SOCKET sock; // is also an id
   char name[SMALL_SIZE];
   char state; // m : main menu, u : user list, g : game list, p : playing
   char description[BUF_SIZE];
   int subscribedGame;
} Client;

#endif /* guard */
