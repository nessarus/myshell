#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Compile this program with:
//      cc -std=c99 -Wall -Werror -pedantic -o rot rot.c

//#define ROT 3

//  The rotate function returns the character ROT positions further along the
//  alphabetic character sequence from c, or c if c is not lower-case

char rotate(char c, int ROT)
{
        // Check if c is lower-case or not
        if (islower(c))
        {
                // The ciphered character is ROT positions beyond c,
                // allowing for wrap-around
                return ('a' + (c - 'a' + ROT) % 26);
        }
	else if (isupper(c))
	{
		return ('A' + (c - 'A' + ROT) % 26);
	}
	else if (isdigit(c))
	{
		return ('0' + (c - '0' + ROT) % 10);
	}
        else
        {
                return c;
        }
}

//  Execution of the whole program begins at the main function

int main(int argc, char *argv[])
{
        int rot = 13;

	// Exit with an error if the number of arguments (including
        // the name of the executable) is not precisely 2
        if(argc < 2)
        {
                fprintf(stderr, "%s: program expected ROT number and arguments, but instead received %d\n", argv[0], argc-1);
                exit(EXIT_FAILURE);
        }
	if(argc == 2)
	{
		if(strlen(argv[1])==1 && isdigit(argv[1][0]))
		{
			fprintf(stderr, "%s: program expected ROT number, but instead received %d\n", argv[0], argc-1);
			exit(EXIT_FAILURE);
		}
	}

	int index = 1;

	if(strlen(argv[1])==1 && isdigit(argv[1][0]))
	{
		rot = atoi(argv[1]);
		index++;
	}

	for(int i=index; i < argc; i++)
	{
		// Calculate the length of the first argument
		int length = strlen(argv[i]);
		
		// Print out orginal first
		// Loop for every character in the text
		for(int j = 0; j < length; j++)
		{
			printf("%c %d\n", argv[i][j], (int) argv[i][j]);

		}
		
		// Separate orginal from rot
		printf("\n");
	}
	
	for(int i=index; i < argc; i++)
	{
		// Calculate the length of the first argument
		int length = strlen(argv[i]);

		// Print out rot
		// Loop for every character in the text
		for(int j = 0; j < length; j++)
		{
			// Determine and print the ciphered character
			printf("%c %d\n", rotate(argv[i][j], rot), (int) rotate(argv[i][j], rot));
		}

		// Print one final new-line character
		printf("\n");
	}

	// Exit indicating success
	exit(EXIT_SUCCESS);


        return 0;
}
