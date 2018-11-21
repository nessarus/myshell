#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// testing existIn 
/**
 * existsIn receives a string and checks if it is in 
 * the array. It returns the position in the array.
 */ 
int existsIn(char *string, char *array[], int length) {
    for(int i=0; i<length; i++) {
        if(strcmp(string, array[i]) ==0){
            return i;
        }
    }
    return -1;
}

// sumbytes should remove duplicate devices and 
// sum the bytes. It also should return the number
// of devices left.
void sumbytes(char *string, int bytes, 
        char *dev[], int *byt, int *length) {
    
    int exist = existsIn(string, dev, *length);
    if(exist == -1){
        dev[*length] = string;
        byt[*length] = bytes;
        *length = *length + 1;
    }
    else {
        byt[exist] += bytes;
    }
}

int main(void)
{
    char *array[4] = {"one", "two", "three"};
    char *string = "four";
    int byt[4] = {1, 2, 3};
    int byte = 33;
    int length = 3;
    sumbytes(string, byte, array, byt, &length);
    printf("%s, %d, %d\n", array[0], byt[0], length);
    printf("%s, %d, %d\n", array[1], byt[1], length);
    printf("%s, %d, %d\n", array[2], byt[2], length);
    printf("%s, %d, %d\n", array[3], byt[3], length);
    return 0;
}
