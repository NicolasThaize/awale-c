#include "testList.h"
#include "../src/server/list/list.h"


static void subscribeToDiffusion(List *diffusion, int *socketId) {
   addElement(diffusion, socketId);
}

int main(int argc, char **argv) {
   List diffusionMainMenu; // identifiant client
   initializeList(&diffusionMainMenu);


   return 0;
}