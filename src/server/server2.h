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

#define green	"\033[0;32m"
#define white	"\033[0;37m"
#define red		"\033[0;31m"
#define yellow	"\033[0;33m"
#define blue	"\033[0;36m"

// --------------- app ---------------

static void app(void);

// --------------- diffusion ---------------

static void unsubscribeFromDiffusion(int diffusion[], int max_size, int socketId);
static void subscribeToDiffusion(int diffusion[], int max_size, int socketId);
static void switchDiffusion(Client *client, char to, int diffusionMainMenu[MAX_CLIENTS], int diffusionUsersList[MAX_CLIENTS], int diffusionGamesList[MAX_CLIENTS], int *diffusionOfTheGame);
static void sendToDiffusion(Client diffusion[], char string[BUF_SIZE]);

// --------------- show ---------------

static void showBoard(const Client client, const int board[]);
static void showMoveRequest(Client client);
static void showChallenge(char challengerName[SMALL_SIZE], Client client);
static void showHelp(Client client);
static void showMenu(Client client);
static int showUserList(Client client, Client listAllClients[MAX_CLIENTS]);
static int showGameList(Client client, Game gameList[MAX_GAMES], Client clientList[MAX_CLIENTS]);
static void showOtherPlayer(Client clientTo, Client otherClient);

// --------------- select ---------------

static Client getClient(const int id, const Client allClients[MAX_CLIENTS], const int nbClients);
static Client* findClient(Client clients[MAX_CLIENTS], const char name[SMALL_SIZE]);
static int getSocketIdByUsername(const char username[SMALL_SIZE], const Client listAllClient[MAX_CLIENTS]);
static Client getClientBySocketId(const Client allClients[MAX_CLIENTS], const int socketId);

// --------------- from list ---------------

static Client userFromList(int input, Client listAllClients[]);
static int gameFromList(int input, Game listAllGames[]);

// --------------- game ---------------

static int findEmptyGame(Game gameList[]);
static int findGame(Game gameList[], int challenger, int challenged);
static Game* getClientGame(Client client, const Game gameList[]);

// --------------- play ---------------

static int play(Game* g, int computerMove);

// --------------- other ---------------

static char convert(char userInput);

#endif /* guard */
