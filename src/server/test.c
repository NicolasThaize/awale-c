#include <time.h>
#include <stdlib.h>
#include <stdio.h>

static int convert(int userInput) {
   int res;
   if ( userInput < 13 && userInput > 6 ) {
      res = 12 - userInput;
   } else if ( userInput > 0 && userInput < 7 ) {
      res = userInput + 5;
   }
   else {
      res = -1;
   }
   return res;
}

int nextArrayElem(int baseIndex) {
    if (baseIndex == 0) {
        return 6;
    } else if (baseIndex == 11) {
        return 5;
    } else if (baseIndex < 6 && baseIndex > 0) {
        return baseIndex - 1;
    } else if (baseIndex < 12 && baseIndex > 5) {
        return baseIndex + 1;
    } else {
        return -1;
    }    
}

char process(const int *baseArray, int *outArray, int userInput) {
    for (int i = 0; i < 12; i++) {
        outArray[i] = baseArray[i];
    }
    
    int correspondingArrayIndex = convert(userInput);

    //printf("User input: %d, corresponding: %d \n", userInput, correspondingArrayIndex);
    
    //printf("Base array : \n");
    //for(int i = 0; i < 12; i++) {
    //    printf("%d ", baseArray[i]);
    //}

    int valueToAdd = baseArray[correspondingArrayIndex];

    outArray[correspondingArrayIndex] = 0;
    //printf("\n Value to add: %d \n", valueToAdd);


    for (int i = valueToAdd; i > 0; i--) {
        int indexToAddIn = nextArrayElem(correspondingArrayIndex);
        
        //printf("Add 1 in case: %d ", indexToAddIn);
        
        outArray[indexToAddIn] = outArray[indexToAddIn] + 1;
        correspondingArrayIndex = correspondingArrayIndex + 1;
    }

    //printf("\n New array : \n");
    //for(int i = 0; i < 12; i++) {
    //    printf("%d ", outArray[i]);
    //}

    return 1;
}


int main() {
    int baseArray[12] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
    int outArray[12];
    int userInput = 3;
    
    // Process prend un tableau en entrée et une position (valeur input du joueur). Joue le coup et renvoie le tableau
    process(baseArray, outArray, userInput);
}
