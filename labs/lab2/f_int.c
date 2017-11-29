#include <stdio.h>
#include <stdlib.h>

// Write a program which accepts exactly 3 command-line arguments, 
// converts each to an integer value, and prints the maximum of the 3 values.

int main(int argc, char *argv[]) {
	if(argc < 2) {
		fprintf(stderr, "%s: program expected arguments, but recieved %d\n", argv[0], argc-1);
	} else {
		int value[argc-1];
		for(int i=0; i<(argc-1);i++){
			value[i] = atoi(argv[i+1]);
		}

		int max=value[0];
		for(int i=1; i<(argc-1); i++) {
			if(value[i]>max) {
				max = value[i];
			}
		}
		printf("%d\n", max);
	}
	return 0;
}
