#include <stdio.h>
#include <stdlib.h>

// Write a program that prints out the ordinal description of the
// (assumed to be numerical) arguments supplied to the program. 

int main(int argc, char *argv[]) {
	if(argc < 2) {
		fprintf(stderr, "%s: program expected arguments, but received %d\n", argv[0], argc-1);
	} else {
		int value[argc-1];
		for(int i=0; i<(argc-1);i++){
			value[i] = atoi(argv[i+1]);
		}

		for(int i=0; i<(argc-1); i++) {
			if(value[i]%10==1) {
				printf("%dst\n", value[i]);
			} else if(value[i]%10==2){
				printf("%dnd\n", value[i]);
			} else if(value[i]%10==3){
				printf("%drd\n", value[i]);
			} else {
				printf("%dth\n", value[i]);
			}
		}
	}
	return 0;
}
