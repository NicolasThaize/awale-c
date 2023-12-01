#ifndef SERVER_H
#define SERVER_H

#include "client.h"
#include "challenge.h"
#include "game.h"
#include "network.h"

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

#define CRLF        "\r\n"
#define BUF_SIZE    1024
#define SMALL_SIZE	32

#define MAX_CLIENTS     100
#define MAX_GAMES	500

#define IMPOSSIBLE_ID -1
#define MAIN_MENU 'm'
#define USER_LIST 'u'
#define GAME_LIST 'g'
#define GAME 'p'

// --------------- app ---------------

static void app(void);

// --------------- diffusion ---------------

static void unsubscribeFromDiffusion(int diffusion[], int max_size, int socketId);
static void subscribeToDiffusion(int diffusion[], int max_size, int socketId);
static void switchDiffusion(char from, char to, int socketId, int subscribedGame, int diffusionMainMenu[MAX_CLIENTS], int diffusionUsersList[MAX_CLIENTS], int diffusionGamesList[MAX_CLIENTS], int diffusionGames[MAX_CLIENTS]);

// --------------- show ---------------

static void showHelp(Client client);
static void showMenu(Client client);
static void showUserList(Client client, Client listAllClients[]);
static void showGameList(Client client, Game gameList[], Client clientList[]);

// --------------- select ---------------

static Client getClient(int id, Client allClients[], int nbClients);
static Client findClient(Client *clients, int numClient, const char* name);
static int getSocketIdByUsername(const char username[], const Client listAllClient[]);

// --------------- from list ---------------

static Client userFromList(Client client, int input, Client listAllClients[]);
static Game gameFromList(Client client, int input, Game listAllGames[]);

// --------------- game ---------------

static int findEmptyGame(Game gameList[]);
static int findGame(Game gameList[], int challenger, int challenged);
static Game getClientGame(Client client, const Game gameList[]);

// --------------- play ---------------

static int play(Game* g, int computerMove);

// --------------- other ---------------

static char convert(char userInput);

#endif /* guard */
