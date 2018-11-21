#include <stdio.h>
#include <stdlib.h>

/* a program to display the following patterns of asterisks:
	One line containing 10 asterisks: **********
	10 lines, each line containing 1 asterisk.
	10 lines, each line containing 5 asterisks (i.e. a filled rectangle of 50 asterisks).
*/

int main(int argc, char *argv[]) {
	for(int i=0; i<6;i++){
		for(int j=0; j<(6-i);j++){
			printf(" ");
		}
		for(int j=0;j<i;j++){
			printf("*");
		}
		printf("\n");
	}

	return 0;
}
