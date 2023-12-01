#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server2.h"
#include "client2.h"
#include "challenge.h"
#include "game/game.h"

#ifdef TRACE
#define debug(expression) (printf("%s:%d -> " #expression "\n",__FILE__,__LINE__))
#define debugd(expression) (printf("%s:%d -> %d " #expression "\n",__FILE__,__LINE__,expression))
#define debugc(expression) (printf("%s:%d -> %c " #expression "\n",__FILE__,__LINE__,expression))
#else
#define debug(expression) ((void)0)
#define debugd(expression) ((void)0)
#define debugc(expression) ((void)0)
#endif

static void init(void) {
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

static void end(void) {
#ifdef WIN32
   WSACleanup();
#endif
}

static void unsubscribeFromDiffusion(int *diffusion, int max_size, int socketId) {
   for(int i = 0;  i < max_size; i++){
      if (diffusion[i] == socketId) {
         diffusion[i] = IMPOSSIBLE_ID;
         return;
      }
   }
}

static void subscribeToDiffusion(int *diffusion, int max_size, int socketId) {
   for(int i = 0;  i < max_size; i++){
      if (diffusion[i] == IMPOSSIBLE_ID) {
         diffusion[i] = socketId;
         return;
      }
   }
 }


static void switchDiffusion(char from, char to, int socketId, int subscribedGame, int *diffusionMainMenu, int *diffusionUsersList, int *diffusionGamesList, int *diffusionGames) {
   switch (from) {
      case MAIN_MENU:
         unsubscribeFromDiffusion(diffusionMainMenu, MAX_CLIENTS, socketId);
         break;
      case USER_LIST:
         unsubscribeFromDiffusion(diffusionUsersList, MAX_CLIENTS, socketId);
         break;
      case GAME_LIST:
         unsubscribeFromDiffusion(diffusionGamesList, MAX_CLIENTS, socketId);
         break;
      case GAME:
         unsubscribeFromDiffusion(&diffusionGames[subscribedGame], MAX_CLIENTS, socketId);
         break;
      
      default:
         break;
   }

   switch (to){
   case MAIN_MENU:
         subscribeToDiffusion(diffusionMainMenu, MAX_CLIENTS, socketId);
         break;
      case USER_LIST:
         subscribeToDiffusion(diffusionUsersList, MAX_CLIENTS, socketId);
         break;
      case GAME_LIST:
         subscribeToDiffusion(diffusionGamesList, MAX_CLIENTS, socketId);
         break;
      case GAME:
         subscribeToDiffusion(&diffusionGames[subscribedGame], MAX_CLIENTS, socketId);
         break;
      
      default:
         break;
   }
}

static int getSocketIdByUsername(const char *username, const Client *listAllClient) {
   for (int i = 0; i < MAX_CLIENTS; i++) {
      if (listAllClient[i].name == username) {
         return listAllClient[i].sock;
      }
   }
}

static void app(void) {
   SOCKET sock = init_connection();
   char buffer[BUF_SIZE];
   char name[SMALL_SIZE];
   int number;
   /* the index for the array */
   int nbClients = 0;
   int max = sock;
   /* an array for all clients */
   Client listAllClients[MAX_CLIENTS]; // listAllClients
   int diffusionMainMenu[MAX_CLIENTS] = { [0 ... MAX_CLIENTS-1] = IMPOSSIBLE_ID }; // identifiant client
   int diffusionUsersList[MAX_CLIENTS] = { [0 ... MAX_CLIENTS-1] = IMPOSSIBLE_ID };
   int diffusionGamesList[MAX_CLIENTS] = { [0 ... MAX_CLIENTS-1] = IMPOSSIBLE_ID };
   int diffusionGames[MAX_GAMES][MAX_CLIENTS] = { [0 ... MAX_GAMES-1] = { [0 ... MAX_CLIENTS-1] = IMPOSSIBLE_ID } };
   Challenge listOfChallenges[MAX_GAMES];

   fd_set rdfs;

   while (1) {
      int i = 0;
      FD_ZERO(&rdfs);

      /* add STDIN_FILENO */
      FD_SET(STDIN_FILENO, &rdfs);

      /* add the connection socket */
      FD_SET(sock, &rdfs);

      /* add socket of each client */
      for (i = 0; i < nbClients; i++) {
         FD_SET(listAllClients[i].sock, &rdfs);
      }

      if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1) {
         perror("select()");
         exit(errno);
      }

      /* something from standard input : i.e keyboard */
      if (FD_ISSET(STDIN_FILENO, &rdfs)) {
         /* stop process when type on keyboard */
         break;
      } else if (FD_ISSET(sock, &rdfs)) {
         /* new client */
         SOCKADDR_IN csin = { 0 };
         socklen_t sinsize = sizeof csin;
         int csock = accept(sock, (SOCKADDR *)&csin, &sinsize);
         if(csock == SOCKET_ERROR) {
            perror("accept()");
            continue;
         }

         /* after connecting the client sends its name */
         if (read_client(csock, buffer) == -1) {
            /* disconnected */
            continue;
         }

         /* what is the new maximum fd ? */
         max = csock > max ? csock : max;

         FD_SET(csock, &rdfs);

         Client c = { csock };
         strncpy(c.name, buffer, SMALL_SIZE - 1);
         listAllClients[nbClients] = c;
         nbClients++;
      } else {
         for (int i=0; i<nbClients; i++) {
            /* a client is talking */
            if ( FD_ISSET(listAllClients[i].sock, &rdfs) ) {
               Client client = listAllClients[i];
               int c = read_client(listAllClients[i].sock, buffer);
               /* client disconnected */
               if (c == 0) {
                  closesocket(listAllClients[i].sock);
                  remove_client(listAllClients, i, &nbClients);
                  strncpy(buffer, client.name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  send_message_to_all_clients(listAllClients, client, nbClients, buffer, 1);
               }
               else {
                  // Client* diffusion;
                  if ( buffer[0] == '/' ) {
                     switch (buffer[1]) {
                     case 'y': // yes
                        if (sscanf(buffer, "/y %s", name) == 1) {
                           printf("Name: %s\n", name);
                           // find the corresponding game
                           // change active attribute of game to 1
                        } else {
                           debug("Problem sscanf");
                        }
                        break;
                     case 'n': // no
                        if (sscanf(buffer, "/n %s", name) == 1) {
                           printf("Name: %s\n", name);
                           // find the corresponding game
                           // remove it from the list
                        } else {
                           debug("Problem sscanf");
                        }
                        break;
                     case 'c': // chat
                        if (sscanf(buffer, "/c %[^\n]", buffer) == 1) {
                           printf("Remaining: %s\n", buffer);
                           send_message_to_all_clients(listAllClients, client, nbClients, buffer, 0);
                        } else {
                           debug("Problem sscanf");
                        }
                        break;
                     case 'q': // quit
                        // write_client(client.sock, "You just disconnected from server");
                        // if (FD_ISSET(client.sock, &rdfs)) {
                        //    printf("Bazouzou");
                        //    FD_CLR(client.sock, &rdfs);
                        // }
                        // printf("Bazouzou2");
                        printf("Bazou");
                        if (client.state == 'm') {
                           close(client.sock);
                        } else {
                           switchDiffusion(client.state,MAIN_MENU,client.sock,client.subscribedGame,diffusionMainMenu,diffusionUsersList,diffusionGamesList,diffusionGames);
                        }
                        break;
                     case 'h': // help
                        showHelp(client);
                        break;
                     case 'a': // abandon
                        if (client.state == GAME)
                        break;
                     default:
                        // diffusion = listAllClients;
                        // sprintf(buffer, "%s use unknown / command", client.name);
                        strncpy(buffer, client.name, BUF_SIZE-1);
                        strncat(buffer, " use unknown / command", sizeof buffer - strlen(buffer)-1);
                        break;
                     }
                  } else if (sscanf(buffer, "%d", &number) == 1) {
                     printf("Number: %d\n", number);
                     debugd(number);
                     switch (client.state) {
                        case MAIN_MENU:
                           if (number == 0) {
                              // quit
                           } else if (number == 1) {
                              // play
                           } else if (number == 2) {
                              // TODO : Implémenter quand il y aura une gamelist showGameList(client, gameList, clientList);
                              switchDiffusion(client.state,GAME_LIST,client.sock,client.subscribedGame,diffusionMainMenu,diffusionUsersList,diffusionGamesList,diffusionGames);
                           } else {
                              debug("Bad number from main menu");
                           }
                        case USER_LIST:
                           select_user_from_list(client,number,listAllClients);
                           break;
                        case GAME_LIST:
                            // TODO : Implémenter quand il y aura une gamelist select_game_from_list(client,number, gameList);
                           break;
                        case GAME:
                           // test if the player is in a game and get the game
                           // TODO : Implémenter quand il y aura une gamelist Game g = getClientGame(client, listOfGames);
                           play(client,number); // to implement (arguments surely missing)
                           break;
                        default:
                           debugc(client.state);
                           break;
                     }
                  } else {
                     debug("Problem !!");
                  }
               }
               break;
            }
         }
      }
   }

   clear_clients(listAllClients, nbClients);
   end_connection(sock);
}

static Game getClientGame(Client client, const Game *gameList) {
   if (client.subscribedGame != IMPOSSIBLE_ID) {
      if (gameList[client.subscribedGame].challenged == client.sock || gameList[client.subscribedGame].challenger == client.sock) {
         return gameList[client.subscribedGame];
      }
   }
   debugd("No game found");
}

void showHelp(Client client) {
   char buffer[BUF_SIZE]; 
   strcat(buffer, "/h\t\t\tshow this help\n");
   strcat(buffer, "/c {message}\tsend your message\n");
   strcat(buffer, "/y {player}\taccept challenge of the player {player}\n");
   strcat(buffer, "/n {player}\trefuse challenge of the player {player}\n");
   strcat(buffer, "/a\t\t\tgive up the game\n");
   strcat(buffer, "/q\t\t\tgo back the previous menu (don't abandon in game)\n");
   strcat(buffer, "{N}\t\t\texecute the action number {N}\n");

   write_client(client.sock, buffer);
}

Client getClient(int id, Client allClients[], int nbClients) {
   for (int i=0; i<nbClients; i++) {
      if (allClients[i].sock == id) {
         return allClients[i];
      }
   }
   debug("Id not found");
   // find smth to return
}

static Client select_user_from_list(Client client, int input, Client *listAllClients) {
   int nbExistingUsers = 1;

   for (int i = 0; i < MAX_CLIENTS; i++) {
      if (listAllClients[i].sock != IMPOSSIBLE_ID)
      {
         if (nbExistingUsers == input) {
            return listAllClients[i];
         }
         nbExistingUsers++;
      }
   }
}
static Game select_game_from_list(Client client, int input, Game *listAllGames) {
   int nbExistingGames = 1;

   for (int i = 0; i < MAX_CLIENTS; i++) {
      if (listAllGames[i].active == 1)
      {
         if (nbExistingGames == input) {
            return listAllGames[i];
         }
         nbExistingGames++;
      }
   }
}

static void showUserList(Client client, Client *listAllClients) {
   char usersList[MAX_CLIENTS * SMALL_SIZE + 100] = "Liste des utilisateurs :\n";
   int nbUsers = 0;

   for (int i = 0; i < MAX_CLIENTS; i++) {
      if (listAllClients[i].sock != IMPOSSIBLE_ID) {
         nbUsers++;
         char nbUsersChars[SMALL_SIZE];
         sprintf(nbUsersChars, "%d", nbUsers);

         strcat(strcat(usersList, strcat(strcat(nbUsersChars, ". "), listAllClients[i].name)), "\n");
      }
   }

   write_client(client.sock, usersList);
}

static void showGameList(Client client, Game *gameList, Client *clientList) {
   char gamesList[MAX_CLIENTS * SMALL_SIZE + 100] = "Liste des parties :\n";
   int nbGames = 0;

   for (int i = 0; i < MAX_CLIENTS; i++) {
      if (gameList[i].active == 1) {
         nbGames++;
         Client client1 = getClient(gameList[i].challenged, clientList, MAX_CLIENTS);
         Client client2 = getClient(gameList[i].challenger, clientList, MAX_CLIENTS);
         char nbGamesChars[SMALL_SIZE];
         sprintf(nbGamesChars, "%d", nbGames);
         strcat(strcat(gamesList, strcat(strcat(nbGamesChars, ". "), strcat(strcat(client1.name, "vs. "), client2.name))), "\n");
      }
   }
   
   write_client(client.sock, gamesList);
}


static void play(Client client, int input) {
   // char board[12] = {6,5,4,0,0,6,6,1,0,7,7,6}; // int sur 1 octet
   // show_board(board);
   // char aa;
   // scanf("select case : %c", &aa);
   // printf("You have selected ");
   // printf("%c",convert(aa));

   // to do
}

// static void show_board(const char* board) {
//    printf("\n");
//    for (int i=12; i>9; i--) {
//       printf("  %d",i);
//    }
//    for (int i=9; i>6; i--) {
//       printf("  %d ",i);
//    }
//    printf("\n");
//    for (int i=0; i<6; i++)
//       printf(" ---");
//    printf("\n");
//    for (int i=0; i<6; i++) {
//       if (board[i] > 9) {
//          printf("|%d ",board[i]);
//       } else {
//          printf("| %d ",board[i]);
//       }
//    } printf("|\n");
//    for (int i=0; i<6; i++)
//       printf(" ---");
//    printf("\n");
//    for (int i=0; i<6; i++) {
//       if (board[i] > 9) {
//          printf("|%d ",board[i+6]);
//       } else {
//          printf("| %d ",board[i]);
//       }
//    } printf("|\n");
//    for (int i=0; i<6; i++)
//       printf(" ---");
//    printf("\n");
//    for (int i=0; i<6; i++) {
//       printf("  %d ",i+1);
//    }
//    printf("\n\n");
// }

static char convert(char userInput) {
   char res;
   if ( userInput<7 && userInput>0 ) {
      res = userInput-7;
   } else if ( userInput<13 ) {
      res = 13-userInput;
   }
   else {
      debugc(userInput);
      res = -1;
   }
   return res;
}

static Client find_client(Client *clients, int numClients, const char* name) {
   for (int i=0; i<numClients; i++) {
      if ( strcmp(clients[i].name,name) == 0 ) {
         return clients[i];
      }
   }
   debug("No clients found !");
   return clients[0]; // find a better stop case
}

static void clear_clients(Client *clients, int nbClients) {
   int i = 0;
   for(i = 0; i < nbClients; i++) {
      closesocket(clients[i].sock);
   }
}

static void remove_client(Client *clients,
                        int to_remove,
                        int *nbClients) {
   /* we remove the client in the array */
   memmove(clients + to_remove,
         clients + to_remove + 1,
         (*nbClients - to_remove - 1) * sizeof(Client));
   /* number client - 1 */
   (*nbClients)--;
}

static void send_message_to_all_clients(Client *clients,
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
         write_client(clients[i].sock, message);
      }
   }
}

static int init_connection(void) {
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

static void end_connection(int sock) {
   closesocket(sock);
}

static int read_client(SOCKET sock, char *buffer) {
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0) {
      perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   return n;
}

static void write_client(SOCKET sock, const char *buffer) {
   if(send(sock, buffer, strlen(buffer), 0) < 0) {
      perror("send()");
      exit(errno);
   }
}

int main(int argc, char **argv) {

   init();

   app();

   end();

   return EXIT_SUCCESS;
}
