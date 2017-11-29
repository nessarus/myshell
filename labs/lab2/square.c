#include <stdio.h>
#include <stdlib.h>

//  Compile with:  cc -std=c99 -Wall -Werror -pedantic -o square square.c

void square(int number)
{
    printf("The square of %i is %i\n", number, number*number);
}

int main(int argc, char *argv[])
{
    // Check the number of command-line arguments
    if(argc < 2)
    {
        fprintf(stderr, "Usage: %s value1 [value2 ...]\n", argv[0]);
        exit(EXIT_FAILURE);         // Exit indicating failure
    }
    else
    {
        // Loop over each argument passed to the program
        for(int a = 1 ; a < argc ; a = a+1)
        {
            int n = atoi(argv[a]);  // Convert the a'th argument

            square(n);              // Call our function
        }

        exit(EXIT_SUCCESS);         // Exit indicating success
    }
    return 0;
}
