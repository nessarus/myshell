#include <stdio.h>
#include <stdlib.h>
 

/* A year is a leap year if it is divisible by 400, or if it is divisible by 4 and not divisible by 100. 
 * For example - 1996, 2000, and 2012 were leap years, while 1899, 1900, and 2013 were not. Write a 
 * program that determines if the year, supplied as a command-line argument, is a leap year or not.
 */


int main(int argc, char *argv[]) {
	if(argc < 2) {
		fprintf(stderr, "%s: program expected one argument, but recieved %d\n", argv[0], argc-1);
	} else {
		int year = atoi(argv[1]);
		if (year%400==0) {
			printf("%d is a leap year.\n", year);
		} else if((year%4==0)  && (year%100!=0)){
			printf("%d is a leap year.\n", year);
		} else {
			printf("%d is not a leap year.\n", year);
		}
	}
	return 0;
}
