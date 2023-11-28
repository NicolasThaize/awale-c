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

int previousArrayElem(int baseIndex) {
    if (baseIndex == 6) {
        return 0;
    } else if (baseIndex == 5) {
        return 11;
    } else if (baseIndex < 6 && baseIndex > 0) {
        return baseIndex + 1;
    } else if (baseIndex < 12 && baseIndex > 5) {
        return baseIndex - 1;
    } else {
        return -1;
    }    
}

int checkPrendre(int *array, int index) {
    if (array[index] == 2 || array[index] == 3)
    {
        const int value = array[index];
        array[index] = 0;
        return value;
    }
    return -1;
}

int process(const int *baseArray, int *outArray, int userInput) {
    int grainesRecuperees = 0;
    
    for (int i = 0; i < 12; i++) {
        outArray[i] = baseArray[i];
    }
    
    int correspondingArrayIndex = convert(userInput);
    const int baseArrayIndex = correspondingArrayIndex;

    //printf("User input: %d, corresponding: %d \n", userInput, correspondingArrayIndex);
    
    //printf("Base array : \n");
    //for(int i = 0; i < 12; i++) {
    //    printf("%d ", baseArray[i]);
    //}

    int valueToAdd = baseArray[correspondingArrayIndex];

    outArray[correspondingArrayIndex] = 0;
    //printf("\n Value to add: %d \n", valueToAdd);


    for (int i = valueToAdd; i > 0; i--) {
        
        correspondingArrayIndex = nextArrayElem(correspondingArrayIndex);
        
        //printf("Add 1 in case: %d ", correspondingArrayIndex);
        if (correspondingArrayIndex == baseArrayIndex) {
            i++;
            continue;
        }
        outArray[correspondingArrayIndex] = outArray[correspondingArrayIndex] + 1;
    }

    

    int valueToGive = checkPrendre(outArray, correspondingArrayIndex);
    while (valueToGive != -1) {
        grainesRecuperees = grainesRecuperees + valueToGive;
        correspondingArrayIndex = previousArrayElem(correspondingArrayIndex);
        valueToGive = checkPrendre(outArray, correspondingArrayIndex);
    }
    
    //printf("\n New array : \n");
    //for(int i = 0; i < 12; i++) {
    //    printf("%d ", outArray[i]);
    //}

    return grainesRecuperees;
}


int main() {
    int baseArray[12] = {4, 4, 4, 4, 1, 1, 4, 4, 4, 4, 4, 4};
    int outArray[12];
    int userInput = 4;

    int scoreA = 0;
    int scoreB = 0;
    char currentPlayer = 'B';
    
    // Process prend un tableau en entrée et une position (valeur input du joueur). Joue le coup et renvoie le tableau
    printf("%d", process(baseArray, outArray, userInput));
}
