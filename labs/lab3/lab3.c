#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

int my_strlen(char str1[]) {
	return strlen(str1);
}

int numberOfVowels(char str1[]) {
	int num = 0;
	int str_len = strlen(str1);
	char vowels[] = "aeiou";

	for(int i=0; i<str_len; i++) {
		if(strchr(vowels, str1[i])) {
			num++;
		}	
	}
	return num;
}

bool isSafe(char str[]) {
	bool safe = false;
	int str_len = strlen(str);
	int up = 0;
	int lo = 0;
	int di = 0;
	for(int i=0; i<str_len; i++){
		if(isupper(str[i])){
			up++;
		} else if (islower(str[i])){
			lo++;
		} else if (isdigit(str[i])){
			di++;
		} 
	}
	if(up>1 & lo>1 & di>1){
		safe = true;
	}
	return safe;
}

int my_strcmp(char a[], char b[]) {
	int amount;
	int ret;	

	amount = strcmp(a, b);
	
	if(amount <0) {
		ret=-1;
	} else if (amount >0) {
		ret=1;
	} else {
		ret=0;
	}
	return ret;
}
 
bool isPalindrome(char str[]) {
	int len = strlen(str);

	for(int i=0; i<len/2; i++){
		if(str[i] != str[(len-1)-i]){
			return false;
		}
	}
	return true;
}

bool luhn(char str[]) {
	int len = strlen(str);
	int num1 = atoi(str);
	int num2 = num1;

	int sum = 0;
	for(int i = 0; i < len; i=i+2) {
		sum += num1%10;
		num1/=100;
	}
	for(int i = 1; i < len; i+=2) {
		num2/=10;
		sum += 2*num2;
		num2/=10;
	}
	if(sum%10==0){
		return true;
	} else {
		return false;
	}
}

int main(int argc, char *argv[])
{
//  ENSURE THAT PROGRAM HAS CORRECT NUMBER OF ARGUMENTS
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s argument\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    else
    {
//  CALL THE PROCESS FUNCTION AND COLLECT THE RESULT
        printf("%s\n", argv[argc-1]);

	int str_len = my_strlen(argv[1]);
	int vowels = numberOfVowels(argv[1]);
	bool safe = isSafe(argv[1]);
	int comp = 0;
	if (argc == 3) {	
		comp = my_strcmp(argv[1], argv[2]);	
	}
	bool pal = isPalindrome(argv[1]);
	bool luh = luhn(argv[1]);


//  PRINT THE RESULT
        printf("The stringlen is %d\n", str_len);
	printf("Number of Vowels %d\n", vowels);
	if(safe){printf("The password is safe.\n");}
	else{printf("The password is not safe.\n");}
	printf("comp: %d\n", comp);	
	if(pal){printf("It is a palindrome.\n");}
	else{printf("It is not a palindrom.\n");}
	if(luh){printf("It is valid credit card number.\n");}
	else{printf("It is not a valid credit card number.\n");}	

//  TERMINATE PROGRAM, INDICATING SUCCESS
        exit(EXIT_SUCCESS);
    }
    return 0;
}
