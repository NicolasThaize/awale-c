#ifndef NETWORK_H
#define NETWORK_H

#ifdef WIN32

#include <winsock2.h>

#elif (defined __linux__) || (defined __APPLE__)

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h> // added
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

#error not defined for this platform

#endif

#include "client.h"

#define CRLF        "\r\n"
#define BUF_SIZE    1024
#define SMALL_SIZE	32

#define MAX_CLIENTS     100
#define MAX_GAMES	500

void init(void);
void end(void);
int initConnection(void);
void endConnection(int sock);
int readClient(SOCKET sock, char *buffer);
void writeClient(SOCKET sock, const char *buffer);
void sendMessage2clients(Client *clients, Client client, int actual, const char *buffer, char from_server);
void removeClient(Client *clients, int to_remove, int *actual);
void clearClients(Client *clients, int actual);
void modifyPort(int port);

#endif /* guard */
