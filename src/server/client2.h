#ifndef CLIENT_H
#define CLIENT_H

#include "server2.h"

typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   char state; // m : main menu, u : user list, g : game list, p : playing
}Client;

#endif /* guard */
