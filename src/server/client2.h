#ifndef CLIENT_H
#define CLIENT_H

#include "server2.h"

typedef struct {
   SOCKET sock; // is also an id
   char name[SMALL_SIZE];
   char state; // m : main menu, u : user list, g : game list, p : playing
   char description[BUF_SIZE];
} Client;

#endif /* guard */
