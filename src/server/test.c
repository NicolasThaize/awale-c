#include <time.h>
#include <stdlib.h>
#include <stdio.h>



int main() {
    int baseArray[12] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
    int outArray[12];
    int userInput = 3;
    
    // Process prend un tableau en entrée et une position (valeur input du joueur). Joue le coup et renvoie le tableau
    process(baseArray, outArray, userInput);
}
