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

#define exist(g) ((g.active == 0 && g.finished == 1) ? 0 : 1)



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
   Client listAllClients[MAX_CLIENTS] = { [0 ... MAX_CLIENTS-1] = {.sock = IMPOSSIBLE_ID} }; // listAllClients
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

      while(select(max + 1, &rdfs, NULL, NULL, NULL) == -1) {
         max--;
         if (max == 0) {
            perror("select()");
            exit(errno);
         }
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
         c.state = MAIN_MENU;
         listAllClients[nbClients] = c;
         nbClients++;
         debugc(c.state);
         debugd(c.state);
         showMenu(c);
         // showChallenge("mat", c); // just for test
      } else {
         for (int i=0; i<nbClients; i++) {
            /* a client is talking */
            if ( FD_ISSET(listAllClients[i].sock, &rdfs) ) {
               Client* client = &listAllClients[i];
               int c = readClient(listAllClients[i].sock, buffer);
               /* client disconnected */
               if (c == 0) {
                  closesocket(listAllClients[i].sock);
                  removeClient(listAllClients, i, &nbClients);
                  strncpy(buffer, client->name, BUF_SIZE - 1);
                  strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                  sendMessage2clients(listAllClients, *client, nbClients, buffer, 1);
               }
               else {
                  // Client* diffusion;
                  debug("STATE:");
                  debugc(client->state);
                  if ( buffer[0] == '/' ) {
                     switch (buffer[1]) {
                     case 'y': // yes
                        if (sscanf(buffer, "/y %s", name) == 1) {
                           printf("Name: %s\n", name);
                           // find the corresponding game from listOfGames
                           int id = getSocketIdByUsername(name,listAllClients);
                           if (id == -1) {
                              writeClient(client->sock, "Cannot accept a challenge from a disconnected player");
                              continue;
                           }
                           
                           int indice = findGame(listOfGames, id, client->sock);
                           if (indice == -1) {
                              writeClient(client->sock, "Error, cannot accept a challenge that does not exist");
                              continue;
                           }

                           listOfGames[indice].active = 1;
                           listOfGames[indice].finished = 0;
                           // TODO : initialise board and other variables of the game
                           initGame(&listOfGames[indice]);
                           // TODO : put the two player into the diffusion of the game
                           Client* opponent = findClient(listAllClients,name);
                           if ( opponent->sock != listAllClients[MAX_CLIENTS-1].sock ) {
                              switchDiffusion(client,GAME,diffusionMainMenu,diffusionUsersList,diffusionGamesList,diffusionGames[indice]);
                              switchDiffusion(opponent,GAME,diffusionMainMenu,diffusionUsersList,diffusionGamesList,diffusionGames[indice]);
                           } else {
                              debug(name);
                              debug("NOT FOUND !");
                           }
                           // TODO : send the board to the players
                           showBoard(*client, listOfGames[indice].board);
                           showBoard(*opponent, listOfGames[indice].board);
                           show_board(listOfGames[indice].board);
                           // TODO : request the move of the first player
                           if ( listOfGames[indice].currentPlayer == 1 ) {
                              showMoveRequest(*client);
                           } else if ( listOfGames[indice].currentPlayer == 2 ) {
                              showMoveRequest(*opponent);
                           } else {
                              debugd(listOfGames[indice].currentPlayer);
                           }
                        } else {
                           debug("Problem sscanf");
                        }
                        break;
                     case 'n': // no
                        if (sscanf(buffer, "/n %s", name) == 1) {
                           printf("Name: %s\n", name);
                           // find the corresponding game from listOfGames
                           int id = getSocketIdByUsername(name,listAllClients);
                            if (id == -1) {
                              writeClient(client->sock, "Cannot decline a challenge from a disconnected player");
                              continue;
                           }

                           int indice = findGame(listOfGames, id, client->sock);
                           if (indice == -1) {
                              writeClient(client->sock, "Error, cannot decline a challenge that does not exist");
                              continue;
                           }

                           listOfGames[indice].active = 0;
                           listOfGames[indice].finished = 1;

                           // TODO : show to the users that the challenge is refused
                        } else {
                           debug("Problem sscanf");
                        }
                        break;
                     case 'c': // chat
                        if (sscanf(buffer, "/c %[^\n]", buffer) == 1) {
                           printf("Remaining: %s\n", buffer);
                           sendMessage2clients(listAllClients, *client, nbClients, buffer, 0);
                        } else {
                           debug("Problem sscanf");
                        }
                        break;
                     case 'q': // quit
                        if (client->state == 'm') {
                           close(client->sock);
                           max = max - 1;
                        } else {
                           int indice = client->subscribedGame;
                           switchDiffusion(client,MAIN_MENU,diffusionMainMenu,diffusionUsersList,diffusionGamesList,diffusionGames[indice]);
                        }
                        break;
                     case 'h': // help
                        showHelp(*client);
                        break;
                     case 'a': // abandon
                        if (client->state == GAME) {
                           if (client->sock == listOfGames[client->subscribedGame].challenged ||
                                 client->sock == listOfGames[client->subscribedGame].challenged) {
                              // show win
                              listOfGames[client->subscribedGame].active = 0;
                              listOfGames[client->subscribedGame].finished = 1;
                           }
                        }
                        break;
                     default:
                        // diffusion = listAllClients;
                        // sprintf(buffer, "%s use unknown / command", client->name);
                        showHelp(*client);
                        // strncpy(buffer, client->name, BUF_SIZE-1);
                        // strncat(buffer, " use unknown / command", sizeof buffer - strlen(buffer)-1);
                        break;
                     }
                  } else if (sscanf(buffer, "%d", &number) == 1) {
                     printf("Number: %d, State: %c\n", number, client->state);
                     debugd(number);
                     Client cSelected;
                     int indice;
                     switch (client->state) {
                        case MAIN_MENU:
                           debugc(client->state);
                           if (number == 0) {
                              close(client->sock);
                              max = max - 1;
                           } else if (number == 1) {
                              debug("Hello !!");
                              // play
                              if ( showUserList(*client,listAllClients) != 0 ) {
                                 int indice = client->subscribedGame;
                                 switchDiffusion(client,USER_LIST,diffusionMainMenu,diffusionUsersList,diffusionGamesList,diffusionGames[indice]);
                              } else {
                                 showMenu(*client);
                              }
                           } else if (number == 2) {
                              // spectate
                              if ( showGameList(*client,listOfGames,listAllClients) != 0 ) {
                                 int indice = client->subscribedGame;
                                 switchDiffusion(client,GAME_LIST,diffusionMainMenu,diffusionUsersList,diffusionGamesList,diffusionGames[indice]);
                              } else {
                                 showMenu(*client);
                              }
                           } else {
                              debug("Bad number from main menu");
                           }
                           break;
                        case USER_LIST:
                           // find user
                           cSelected = userFromList(number,listAllClients); // how to get the right number ? (the player 2 disapear)
                           // add the game
                           indice = findEmptyGame(listOfGames);
                           debugd(indice);
                           listOfGames[indice].active = 0;
                           listOfGames[indice].finished = 0;
                           listOfGames[indice].challenger = client->sock;
                           listOfGames[indice].challenged = cSelected.sock;
                           printf("%s", cSelected.name);
                           // inform the challenged
                           showChallenge(client->name, cSelected);
                           break;
                        case GAME_LIST:
                           indice = gameFromList(number, listOfGames); // how to get the right number ? (ex: the game 2 disapear)
                           if ( indice != -1 ) {
                              switchDiffusion(client,GAME,diffusionMainMenu,diffusionUsersList,diffusionGamesList,diffusionGames[indice]);
                           }
                           // TODO int indice = findEmptyGame(listOfGames);
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
                           printf("%c", client->state);
                           printf("%d", client->state);
                           debugc(client->state);
                           break;
                     }
                  } else {
                     showHelp(*client);
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


static void switchDiffusion(Client *client, char to, int diffusionMainMenu[MAX_CLIENTS], int diffusionUsersList[MAX_CLIENTS], int diffusionGamesList[MAX_CLIENTS], int *diffusionOfTheGame) {
   switch (client->state) {
      case MAIN_MENU:
         unsubscribeFromDiffusion(diffusionMainMenu, MAX_CLIENTS, client->sock);
         break;
      case USER_LIST:
         unsubscribeFromDiffusion(diffusionUsersList, MAX_CLIENTS, client->sock);
         break;
      case GAME_LIST:
         unsubscribeFromDiffusion(diffusionGamesList, MAX_CLIENTS, client->sock);
         break;
      case GAME:
         unsubscribeFromDiffusion(diffusionOfTheGame, MAX_CLIENTS, client->sock);
         break;
      
      default:
         break;
   }
   printf("from: %c, to: %c\n", client->state, to);
   client->state = to; // change the state of the client
   switch (to){
      case MAIN_MENU:
         subscribeToDiffusion(diffusionMainMenu, MAX_CLIENTS, client->sock);
         break;
      case USER_LIST:
         printf("userlist\n");
         subscribeToDiffusion(diffusionUsersList, MAX_CLIENTS, client->sock);
         break;
      case GAME_LIST:
         subscribeToDiffusion(diffusionGamesList, MAX_CLIENTS, client->sock);
         break;
      case GAME:
         subscribeToDiffusion(diffusionOfTheGame, MAX_CLIENTS, client->sock);
         break;
      
      default:
         break;
   }
}

// --------------- show ---------------

static void showBoard(const Client client, const int board[]) {
	char buffer[BUF_SIZE] = "";
   char tmp[SMALL_SIZE] = "";
	strcat(buffer, "\n");
	for (int i=12; i>9; i--) {
		sprintf(tmp, "  %d",i);
      strcat(buffer,tmp);
	}
	for (int i=9; i>6; i--) {
		sprintf(tmp, "  %d ",i);
      strcat(buffer,tmp);
	}
	strcat(buffer, "\n");
	for (int i=0; i<6; i++)
		strcat(buffer, " ---");
	strcat(buffer, "\n");
	for (int i=0; i<6; i++) {
		if ( board[i] == 1 || board[i] == 2 ) {
         sprintf(tmp, "%s|%s %d%s ",blue,red,board[i],white);
         strcat(buffer,tmp);
      } else if (board[i] > 9) {
			sprintf(tmp, "%s|%s%d%s ",blue,green,board[i],white);
         strcat(buffer,tmp);
		} else {
			sprintf(tmp, "%s|%s %d%s ",blue,yellow,board[i],white);
         strcat(buffer,tmp);
		}
	} sprintf(tmp, "%s|%s\n",blue,white);
	strcat(buffer, tmp);
	for (int i=0; i<6; i++)
		strcat(buffer, " ---");
	strcat(buffer, "\n");
	for (int i=0; i<6; i++) {
		if ( board[i+6] == 1 || board[i+6] == 2 ) {
         sprintf(tmp, "%s|%s %d%s ",blue,red,board[i+6],white);
         strcat(buffer,tmp);
      } else if (board[i+6] > 9) {
			sprintf(tmp, "%s|%s%d%s ",blue,green,board[i+6],white);
         strcat(buffer,tmp);
		} else {
			sprintf(tmp, "%s|%s %d%s ",blue,yellow,board[i+6],white);
         strcat(buffer,tmp);
		}
	} sprintf(tmp, "%s|%s\n",blue,white);
	strcat(buffer, tmp);
	for (int i=0; i<6; i++)
		strcat(buffer, " ---");
	strcat(buffer, "\n");
	for (int i=0; i<6; i++) {
		sprintf(tmp, "  %d ",i+1);
      strcat(buffer,tmp);
	}
	strcat(buffer, "\n\n");

   writeClient(client.sock, buffer);
}

static void showMoveRequest(Client client) {
   char buffer[BUF_SIZE] = "";
   strcat(buffer, client.name);
   strcat(buffer, " it's your turn : ");

   writeClient(client.sock, buffer);
}

static void showChallenge(char challengerName[SMALL_SIZE], Client client) {
   char buffer[BUF_SIZE] = "";
   // strcat(buffer, yellow);
   strcat(buffer, challengerName);
   // strcat(buffer, white);
   strcat(buffer, " challenge you !\n");

   strcat(buffer, green);
   strcat(buffer, "/y ");
   strcat(buffer, challengerName);
   strcat(buffer, white);
   // strcat(buffer, blue);
   strcat(buffer, "\tto accept\n");

   strcat(buffer, red);
   strcat(buffer, "/n ");
   strcat(buffer, challengerName);
   strcat(buffer, white);
   // strcat(buffer, blue);
   strcat(buffer, "\tto refuse\n");
   // strcat(buffer, white);

   writeClient(client.sock, buffer);
}

static void showHelp(Client client) {
   char buffer[BUF_SIZE] = "";
   char tmp[SMALL_SIZE*3] = "";
   sprintf(tmp, "%s/h%s\t\t\tshow this help\n",blue,white);
   strcat(buffer, tmp);
   sprintf(tmp, "%s/c {message}%s\t\tsend your message\n",blue,white);
   strcat(buffer, tmp);
   sprintf(tmp, "%s/y {player}%s\t\taccept challenge of the player {player}\n",blue,white);
   strcat(buffer, tmp);
   sprintf(tmp, "%s/n {player}%s\t\trefuse challenge of the player {player}\n",blue,white);
   strcat(buffer, tmp);
   sprintf(tmp, "%s/a%s\t\t\tgive up the game\n",blue,white);
   strcat(buffer, tmp);
   sprintf(tmp, "%s/q%s\t\t\texit or go back the previous menu (without giving up)\n",blue,white);
   strcat(buffer, tmp);
   sprintf(tmp, "%s{N}%s\t\t\texecute the action number {N}\n",blue,white);
   strcat(buffer, tmp);

   writeClient(client.sock, buffer);
}

static void showMenu(Client client) {
   char buffer[BUF_SIZE] = "";
   strcat(buffer, "Welcome to AWALE :\n");
   strcat(buffer, "1. Play\n");
   strcat(buffer, "2. Spectate a game\n");
   strcat(buffer, "0. Disconnect\n");

   writeClient(client.sock, buffer);
}

static int showUserList(Client client, Client listAllClients[MAX_CLIENTS]) {
   printf("Showing user list\n");
   char usersList[MAX_CLIENTS * SMALL_SIZE + 100] = "Liste des utilisateurs :\n";
   int nbUsers = 0;

   for (int i = 0; i < MAX_CLIENTS; i++) {
      if ( listAllClients[i].sock != IMPOSSIBLE_ID && listAllClients[i].sock != client.sock ) { // exist & not itself
         nbUsers++;
         char userChars[SMALL_SIZE*2];
         sprintf(userChars, "%d. %s\n",nbUsers,listAllClients[i].name);

         //strcat(strcat(usersList, strcat(strcat(nbUsers, ". "), listAllClients[i].name)), "\n");
         strcat(usersList, userChars);
      }
      else {
         // debug("IMPOSSIBLE ID");
      }
   }
   if (nbUsers == 0) {
      strcat(usersList, "No connected players\n\n");
   }
   
   writeClient(client.sock, usersList);
   return nbUsers;
}

static int showGameList(Client client, Game gameList[MAX_GAMES], Client clientList[MAX_CLIENTS]) {
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
         strcat(strcat(gamesList, strcat(strcat(nbGamesChars, ". "), strcat(strcat(client1.name, " vs. "), client2.name))), "\n");
      }
   }
   if (nbGames == 0) {
      strcat(gamesList, "No current games\n");
   }

   writeClient(client.sock, gamesList);
   return nbGames;
}

// --------------- select ---------------

static Client getClient(const int id, const Client allClients[MAX_CLIENTS], const int nbClients) {
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

static Client* findClient(Client clients[MAX_CLIENTS], const char name[SMALL_SIZE]) {
   for (int i=0; i<MAX_CLIENTS; i++) {
      if ( strcmp(clients[i].name,name) == 0 ) {
         return &clients[i];
      }
   }
   debug("No clients found !");
   return &clients[MAX_CLIENTS-1];
}

static int getSocketIdByUsername(const char username[SMALL_SIZE], const Client listAllClient[MAX_CLIENTS]) {
   for (int i = 0; i < MAX_CLIENTS; i++) {
      if (strcmp(listAllClient[i].name, username) == 0) {
         return listAllClient[i].sock;
      }
   }
   return -1;
}

// --------------- from list ---------------

static Client userFromList(int input, Client listAllClients[]) {
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

static int gameFromList(int input, Game listAllGames[]) {
   int nbExistingGames = 1;

   for (int i = 0; i < MAX_CLIENTS; i++) {
      if (listAllGames[i].active == 1) {
         if (nbExistingGames == input) {
            return i;
         }
         nbExistingGames++;
      }
   }
   debug("No game found !");
   return -1;
}

// --------------- game ---------------

static int findEmptyGame(Game gameList[]) {
   for (int i=0; i<MAX_GAMES; i++) {
      if (exist(gameList[i]) == 0) { // la game n'existe pas
         debugd(gameList[i].active);
         debugd(gameList[i].finished);
         return i;
      }
   }
   debugd(gameList[0].active);
   debugd(gameList[0].finished);
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




