#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server2.h"

#ifdef TRACE
#define debug(expression) (printf("%s:%d -> " #expression "\n",__FILE__,__LINE__))
#define debugd(expression) (printf("%s:%d -> %d " #expression "\n",__FILE__,__LINE__,expression))
#define debugc(expression) (printf("%s:%d -> %c " #expression "\n",__FILE__,__LINE__,expression))
#else
#define debug(expression) ((void)0)
#define debugd(expression) ((void)0)
#define debugc(expression) ((void)0)
#endif

#define exist(g) ((g.active == 0 && g.finished == 1) ? 1 : 0)



// --------------- app ---------------

static void app(void) {
   SOCKET sock = initConnection();
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
   Game listOfGames[MAX_GAMES] = { [0 ... MAX_GAMES-1] = {.active = 0, .finished = 1} };

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
         if (readClient(csock, buffer) == -1) {
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
         showMenu(c); // TODO
      } else {
         for (int i=0; i<nbClients; i++) {
            /* a client is talking */
            if ( FD_ISSET(listAllClients[i].sock, &rdfs) ) {
               Client client = listAllClients[i];
               int c = readClient(listAllClients[i].sock, buffer);
               /* client disconnected */
               if (c == 0) {
                  closesocket(listAllClients[i].sock);
                  removeClient(listAllClients, i, &nbClients);
                  strncpy(buffer, client.name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  sendMessage2clients(listAllClients, client, nbClients, buffer, 1);
               }
               else {
                  // Client* diffusion;
                  if ( buffer[0] == '/' ) {
                     switch (buffer[1]) {
                     case 'y': // yes
                        if (sscanf(buffer, "/y %s", name) == 1) {
                           printf("Name: %s\n", name);
                           // find the corresponding game from listOfGames
                           int id = getSocketIdByUsername(name,listAllClients);
                           // TODO : handle id = -1
                           int indice = findGame(listOfGames, id, client.sock);
                           listOfGames[indice].active = 1;
                           listOfGames[indice].finished = 0;
                        } else {
                           debug("Problem sscanf");
                        }
                        break;
                     case 'n': // no
                        if (sscanf(buffer, "/n %s", name) == 1) {
                           printf("Name: %s\n", name);
                           // find the corresponding game from listOfGames
                           int id = getSocketIdByUsername(name,listAllClients);
                           // TODO : handle id = -1
                           int indice = findGame(listOfGames, id, client.sock);
                           listOfGames[indice].active = 0;
                           listOfGames[indice].finished = 1;
                        } else {
                           debug("Problem sscanf");
                        }
                        break;
                     case 'c': // chat
                        if (sscanf(buffer, "/c %[^\n]", buffer) == 1) {
                           printf("Remaining: %s\n", buffer);
                           sendMessage2clients(listAllClients, client, nbClients, buffer, 0);
                        } else {
                           debug("Problem sscanf");
                        }
                        break;
                     case 'q': // quit
                        // writeClient(client.sock, "You just disconnected from server");
                        // if (FD_ISSET(client.sock, &rdfs)) {
                        //    printf("Bazouzou");
                        //    FD_CLR(client.sock, &rdfs);
                        // }
                        // printf("Bazouzou2");
                        printf("Bazou");
                        if (client.state == 'm') {
                           close(client.sock);
                        } else {
                           int indice = 2; // TODO
                           switchDiffusion(client.state,MAIN_MENU,client.sock,client.subscribedGame,diffusionMainMenu,diffusionUsersList,diffusionGamesList,diffusionGames[indice]);
                        }
                        break;
                     case 'h': // help
                        showHelp(client);
                        break;
                     case 'a': // abandon
                        if (client.state == GAME) {
                           if (client.sock == listOfGames[client.subscribedGame].challenged ||
                                 client.sock == listOfGames[client.subscribedGame].challenged) {
                              // show win
                              listOfGames[client.subscribedGame].active = 0;
                              listOfGames[client.subscribedGame].finished = 1;
                           }
                        }
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
                              showUserList(client,listAllClients);
                              int indice = -1; // TODO
                              switchDiffusion(client.state,USER_LIST,client.sock,client.subscribedGame,diffusionMainMenu,diffusionUsersList,diffusionGamesList,diffusionGames[indice]);
                           } else if (number == 2) {
                              // spectate
                              showGameList(client,listOfGames,listAllClients);
                              int indice = -1; // TODO
                              switchDiffusion(client.state,GAME_LIST,client.sock,client.subscribedGame,diffusionMainMenu,diffusionUsersList,diffusionGamesList,diffusionGames[indice]);
                           } else {
                              debug("Bad number from main menu");
                           }
                        case USER_LIST:
                           Client cSelected = userFromList(client,number,listAllClients);
                           // TODO
                           break;
                        case GAME_LIST:
                           int indice = findEmptyGame(listOfGames);
                           Game gSelected = gameFromList(client,number, listOfGames);
                           // TODO handle game not found
                           // TODO next
                           break;
                        case GAME:
                           // test if the player is in a game and get the game
                           // TODO : Implémenter quand il y aura une gamelist Game g = getClientGame(client, listOfGames);
                           // TODO handle game not found
                           // TODO : Implémenter quand il y aura une gamelist play(&g, convert(number)); // to implement (arguments surely missing)
                           // TODO : Implémenter quand il y aura une gamelist printf("-------------- TOUR %d ----------------",nbTour);
		                     // TODO : Implémenter quand il y aura une gamelist show_board(g.board);
                           break;
                        default:
                           debugc(client.state);
                           break;
                     }
                  } else {
                     showHelp(client);
                     debug("NaN nor a command");
                  }
               }
               break;
            }
         }
      }
   }

   clearClients(listAllClients, nbClients);
   endConnection(sock);
}

// --------------- diffusion ---------------

static void unsubscribeFromDiffusion(int diffusion[], int max_size, int socketId) {
   for(int i = 0;  i < max_size; i++){
      if (diffusion[i] == socketId) {
         diffusion[i] = IMPOSSIBLE_ID;
         return;
      }
   }
}

static void subscribeToDiffusion(int diffusion[], int max_size, int socketId) {
   for(int i = 0;  i < max_size; i++){
      if (diffusion[i] == IMPOSSIBLE_ID) {
         diffusion[i] = socketId;
         return;
      }
   }
}


static void switchDiffusion(char from, char to, int socketId, int subscribedGame, int diffusionMainMenu[MAX_CLIENTS], int diffusionUsersList[MAX_CLIENTS], int diffusionGamesList[MAX_CLIENTS], int diffusionGames[MAX_CLIENTS]) {
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

// --------------- show ---------------

static void showHelp(Client client) {
   char buffer[BUF_SIZE];
   strcat(buffer, "/h\t\t\tshow this help\n");
   strcat(buffer, "/c {message}\t\tsend your message\n");
   strcat(buffer, "/y {player}\t\taccept challenge of the player {player}\n");
   strcat(buffer, "/n {player}\t\trefuse challenge of the player {player}\n");
   strcat(buffer, "/a\t\t\tgive up the game\n");
   strcat(buffer, "/q\t\t\tgo back the previous menu (don't abandon in game)\n");
   strcat(buffer, "{N}\t\t\texecute the action number {N}\n");

   writeClient(client.sock, buffer);
}

static void showMenu(Client client) {}

static void showUserList(Client client, Client listAllClients[]) {
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

   writeClient(client.sock, usersList);
}

static void showGameList(Client client, Game gameList[], Client clientList[]) {
   char gamesList[MAX_CLIENTS * SMALL_SIZE + 100] = "Liste des parties :\n";
   int nbGames = 0;

   for (int i = 0; i < MAX_CLIENTS; i++) {
      if (gameList[i].active == 1) {
         nbGames++;
         Client client1 = getClient(gameList[i].challenged, clientList, MAX_CLIENTS);
         Client client2 = getClient(gameList[i].challenger, clientList, MAX_CLIENTS);
         // TODO handle cases of client.sock = -1
         char nbGamesChars[SMALL_SIZE];
         sprintf(nbGamesChars, "%d", nbGames);
         strcat(strcat(gamesList, strcat(strcat(nbGamesChars, ". "), strcat(strcat(client1.name, "vs. "), client2.name))), "\n");
      }
   }
   
   writeClient(client.sock, gamesList);
}

// --------------- select ---------------

static Client getClient(int id, Client allClients[], int nbClients) {
   for (int i=0; i<nbClients; i++) {
      if (allClients[i].sock == id) {
         return allClients[i];
      }
   }
   debug("Id not found");
   Client c;
   c.sock = -1;
   return c;
}

static Client findClient(Client clients[], int numClients, const char name[]) {
   for (int i=0; i<numClients; i++) {
      if ( strcmp(clients[i].name,name) == 0 ) {
         return clients[i];
      }
   }
   debug("No clients found !");
   Client c;
   c.sock = -1;
   return c;
}

static int getSocketIdByUsername(const char username[], const Client listAllClient[]) {
   for (int i = 0; i < MAX_CLIENTS; i++) {
      if (listAllClient[i].name == username) {
         return listAllClient[i].sock;
      }
   }
   return -1;
}

// --------------- from list ---------------

static Client userFromList(Client client, int input, Client listAllClients[]) {
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
   debug("No clients found !");
   Client c;
   c.sock = -1;
   return c;
}

static Game gameFromList(Client client, int input, Game listAllGames[]) {
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
   debug("No game found !");
   Game g;
   g.active = 0;
   g.finished = 1;
   return g;
}

// --------------- game ---------------

static int findEmptyGame(Game gameList[]) {
   for (int i=0; i<MAX_GAMES; i++) {
      if (exist(gameList[i]) == 0) { // la game n'existe pas
         return i;
      }
   }
   return -1;
}

static int findGame(Game gameList[], int challenger, int challenged) {
   for (int i=0; i<MAX_GAMES; i++) {
      if ( gameList[i].active == 0 &&
            gameList[i].finished != 1 &&
            gameList[i].challenger == challenger &&
            gameList[i].challenged == challenged ) {
         return i;
      }
   }
   return -1;
}

static Game getClientGame(Client client, const Game gameList[]) {
   if (client.subscribedGame != IMPOSSIBLE_ID) {
      if (gameList[client.subscribedGame].challenged == client.sock || gameList[client.subscribedGame].challenger == client.sock) {
         return gameList[client.subscribedGame];
      }
   }
   debug("No game found !");
   Game g;
   g.active = 0;
   g.finished = 1;
   return g;
}

// --------------- play ---------------

static int play(Game* g, int computerMove) {
	if ( moveOkay(*g,computerMove) == 1 ) {
		debug("improve score and apply move");
		printf("Move okay");
		if ( g->currentPlayer == 1 ) {
			g->scoreA += playMoveAndTake(g->board,g->board,computerMove);
		} else if ( g->currentPlayer == 2 ) {
			g->scoreB += playMoveAndTake(g->board,g->board,computerMove);
		} else {
			debug("current player pb");
		}
		debug("test win");
		if ( ( g->currentPlayer == 1 && g->scoreA > 24 ) || ( g->currentPlayer == 2 && g->scoreB > 24 ) ) {
			g->finished = 1;
			printf("Player %d win !\n",g->currentPlayer);
		}
		debug("test loose");
		if ( empty(g->board,nextPlayer(g->currentPlayer)) && bestMove(g->board, g->currentPlayer) == -1 ) {
			g->finished = 1;
			printf("Player %d win !\n",g->currentPlayer);
		}
		g->currentPlayer = nextPlayer(g->currentPlayer);
	} else if ( moveOkay(*g,computerMove) == 3 ) {
		printf("You want to take everything from your opponent but you can't so you take nothing\n");
		playMove(g->board,g->board,computerMove);
		g->currentPlayer = nextPlayer(g->currentPlayer);
		g->finished = 1;
	} else if ( moveOkay(*g,computerMove) == 2 ) {
		printf("It appear that you have win\n");
		g->finished = 1;
	} else if ( moveOkay(*g,computerMove) == 4) {
		printf("It is impossible to play an empty box\n");
		return 0;
	} else {
		printf("There is an error\n");
		debugd(moveOkay(*g,computerMove));
		g->finished = 1;
	}

	return 1;
}

// --------------- other ---------------

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

int main(int argc, char **argv) {

   int PORT = 1977;
   if (argc == 2) {
      sscanf(argv[1],"%d",&PORT);
      modifyPort(PORT);
   }

   init();

   app();

   end();

   return EXIT_SUCCESS;
}




