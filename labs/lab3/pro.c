#include <stdio.h>
#include <stdlib.h>

int process(char s[])
{
	return 0;
}

int main(int argc, char *argv[]) {
	// ensure correct number of arguements
	if (argc <2 ) {
		fprintf(stderr, "Usage: %s argument\n", argv[0]);
		exit(EXIT_FAILURE);
	} else {
	// Call the process function and collect the result
		int answer = process(argv[1]);

	// Print the result
		printf("The answer is %d\n", answer);

	// Terminate program, indicating success
		exit(EXIT_SUCCESS);
	}
	return 0;
}
