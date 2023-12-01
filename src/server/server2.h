#ifndef SERVER_H
#define SERVER_H

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
#define PORT         1977
#define MAX_CLIENTS     100
#define MAX_GAMES	500

#define BUF_SIZE    1024
#define IMPOSSIBLE_ID -1
// #define IMPOSSIBLE_GAME (Game g; g.active = 0; g.finished = 1;)

#define MAIN_MENU 'm'
#define USER_LIST 'u'
#define GAME_LIST 'g'
#define GAME 'p'
#define SMALL_SIZE	32

static void init(void);
static void end(void);
static void app(void);
static void switchDiffusion(char from, char to, int socketId, int subscribedGame, int *diffusionMainMenu, int *diffusionUsersList, int *diffusionGamesList, int *diffusionGames);
static void subscribeToDiffusion(int *diffusion, int max_size, int socketId);
static void unsubscribeFromDiffusion(int *diffusion, int max_size, int socketId);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(SOCKET sock, char *buffer);
static void write_client(SOCKET sock, const char *buffer);
static void send_message_to_all_clients(Client *clients, Client client, int actual, const char *buffer, char from_server);
static Client find_client(Client *clients, int numClient, const char* name);
static void action(Client client, int input);
static Client select_user_from_list(Client client, int input, Client *listAllClients);
static Game select_game_from_list(Client client, int input, Game *listAllGames);
static void showUserList(Client client, Client *listAllClients);
static void showGameList(Client client, Game *gameList, Client *clientList);
static void play(Client client, int input);
//static void show_board(const char* board);
static char convert(char userInput);
static void remove_client(Client *clients, int to_remove, int *actual);
static void clear_clients(Client *clients, int actual);
void showHelp(Client client);

#endif /* guard */
