#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "network.h"

int PORT = 1977;


void init(void) {
#ifdef WIN32
   WSADATA wsa;
   int err = WSAStartup(MAKEWORD(2, 2), &wsa);
   if(err < 0)
   {
      puts("WSAStartup failed !");
      exit(EXIT_FAILURE);
   }
#endif
}

void end(void) {
#ifdef WIN32
   WSACleanup();
#endif
}

int initConnection(void) {
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET) {
      perror("socket()");
      exit(errno);
   }

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR) {
      perror("bind()");
      exit(errno);
   }

   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR) {
      perror("listen()");
      exit(errno);
   }

   return sock;
}

void endConnection(int sock) {
   closesocket(sock);
}

int readClient(SOCKET sock, char *buffer) {
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0) {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   return n;
}

void writeClient(SOCKET sock, const char *buffer) {
   if(send(sock, buffer, strlen(buffer), 0) < 0) {
      perror("send()");
      exit(errno);
   }
}

void sendMessage2clients(Client *clients,
                                       Client sender,
                                       int nbClients,
                                       const char *buffer,
                                       char from_server) {
   int i = 0;
   char message[BUF_SIZE];
   message[0] = 0;
   for(i = 0; i < nbClients; i++) {
      /* we don't send message to the sender */
      if(sender.sock != clients[i].sock) {
         if(from_server == 0) {
            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);
         }
         strncat(message, buffer, sizeof message - strlen(message) - 1);
         writeClient(clients[i].sock, message);
      }
   }
}

void removeClient(Client *clients,
                        int to_remove,
                        int *nbClients) {
   /* we remove the client in the array */
   memmove(clients + to_remove,
         clients + to_remove + 1,
         (*nbClients - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*nbClients)--;
}

void clearClients(Client *clients, int nbClients) {
   int i = 0;
   for(i = 0; i < nbClients; i++) {
      closesocket(clients[i].sock);
   }
}

void modifyPort(int port) {
   PORT = port;
}







