#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "server2.h"
#include "client2.h"
#include "challenge.h"
#include "list/list.h"

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
   // int diffusionMainMenu[MAX_CLIENTS]; // identifiant client
   // int diffusionUsersList[MAX_CLIENTS];
   // int diffusionGamesList[MAX_CLIENTS];
   // int diffusionGames[MAX_GAMES][MAX_CLIENTS];
   List diffusionMainMenu[MAX_CLIENTS]; // identifiant client
   List diffusionUsersList[MAX_CLIENTS];
   List diffusionGamesList[MAX_CLIENTS];
   List diffusionGames[MAX_GAMES][MAX_CLIENTS];
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
         strncpy(c.name, buffer, BUF_SIZE - 1);
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
                        // int id = 1; // find method for unique game
                        // diffusion = diffusionGames[id];
                        // diffusion[0] = client;
                        // diffusion[0].state = 'p';
                        // diffusion[1] = find_client(listAllClients, nbClients, buffer+3); // find the name
                        // diffusion[1].state = 'p';
                        // sprintf(buffer, "%s use /yes", client.name);

                        // find_client(listAllClients,nbClients,buffer+3);
                        
                        // strncpy(buffer, client.name, BUF_SIZE-1);
                        // strncat(buffer, " use /y", sizeof buffer - strlen(buffer)-1);
                        if (sscanf(buffer, "/y %s", name) == 1) {
                           printf("Name: %s\n", name);
                        }
                        break;
                     case 'n': // no
                        // diffusion = diffusionMainMenu;
                        // sprintf(buffer, "%s use /no", client.name);
                        // strncpy(buffer, client.name, BUF_SIZE-1);
                        // strncat(buffer, " use /n", sizeof buffer - strlen(buffer)-1);
                        if (sscanf(buffer, "/n %s", name) == 1) {
                           printf("Name: %s\n", name);
                        }
                        break;
                     case 'c': // chat
                        // diffusion = listAllClients;
                        // sprintf(buffer, buffer+3); // escape "/m "
                        if (sscanf(buffer, "/c %[^\n]", buffer) == 1) {
                           printf("Remaining: %s\n", buffer);
                        }
                        break;
                     case 'q': // quit
                        //write_client(client.sock, "You just disconnected from server");
                        //if (FD_ISSET(client.sock, &rdfs)) {
                        //   printf("Bazouzou");
                        //   FD_CLR(client.sock, &rdfs);
                        //}
                        //printf("Bazouzou2");
                        //close(client.sock);
                        printf("Bazou");
                        break;
                     case 'h': // help
                        showHelp();
                        break;
                     case 'a': // abandon
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
                     // action(client,atoi(buffer));
                  } else {
                     debug("Problem !!");
                  }
                  send_message_to_all_clients(listAllClients, client, nbClients, buffer, 0);
               }
               break;
            }
         }
      }
   }

   clear_clients(listAllClients, nbClients);
   end_connection(sock);
}

void showHelp() {
   printf("/h\t\t\tshow this help\n");
   printf("/c {message}\tsend your message\n");
   printf("/y {player}\taccept challenge of the player {player}\n");
   printf("/n {player}\trefuse challenge of the player {player}\n");
   printf("/a\t\t\tgive up the game\n");
   printf("/q\t\t\tgo back the previous menu (don't abandon in game)\n");
   printf("{N}\t\t\texecute the action number {N}\n");
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

static void action(Client client, int input) {
   debugd(input);
   switch (client.state) {
   case 'u':
      select_user(client,input);
      break;
   case 'g':
      select_game(client,input);
      break;
   case 'p':
      play(client,input);
      break;
   default:
      play(client,input);
      debugc(client.state);
      break;
   }
}

static void select_user(Client client, int input) {
   printf("Nothing for now\n");
}
static void select_game(Client client, int input) {
   printf("Nothing for now\n");
}
static void play(Client client, int input) {
   char board[12] = {6,5,4,0,0,6,6,1,0,7,7,6}; // int sur 1 octet
   show_board(board);
   char aa;
   scanf("select case : %c", &aa);
   printf("You have selected ");
   printf("%c",convert(aa));
}

static void show_board(const char* board) {
   printf("\n");
   for (int i=12; i>9; i--) {
      printf("  %d",i);
   }
   for (int i=9; i>6; i--) {
      printf("  %d ",i);
   }
   printf("\n");
   for (int i=0; i<6; i++)
      printf(" ---");
   printf("\n");
   for (int i=0; i<6; i++) {
      if (board[i] > 9) {
         printf("|%d ",board[i]);
      } else {
         printf("| %d ",board[i]);
      }
   } printf("|\n");
   for (int i=0; i<6; i++)
      printf(" ---");
   printf("\n");
   for (int i=0; i<6; i++) {
      if (board[i] > 9) {
         printf("|%d ",board[i+6]);
      } else {
         printf("| %d ",board[i]);
      }
   } printf("|\n");
   for (int i=0; i<6; i++)
      printf(" ---");
   printf("\n");
   for (int i=0; i<6; i++) {
      printf("  %d ",i+1);
   }
   printf("\n\n");
}

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




