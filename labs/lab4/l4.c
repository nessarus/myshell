#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

void q2(int argc, char *argv[]) {
	char suit[] = "SHDC";
	char rank[13] = "A2345689TJQK";
	char deck[52][3];
	for(int i=0; i<13; i++) {
		for(int j=0; j<4; j++) {
			deck[4*i+j][0]=rank[i];
			deck[4*i+j][1]=suit[j];
			deck[4*i+j][2]='\0';
		}
	}
	for(int i=0; i<52;i++) {
		printf("%s\n", deck[i]);
	}
}


int con1(char str1[]) {
	if(str1 == NULL) {
		return 0;
	}
	int result = 0;
	int strlen_str1 = strlen(str1);
	if(strlen_str1 == 0) {
		return 0;
	}
	int negative = 0;
	if(str1[0] == '-') {
		negative = 1;
	}
	for(int i=negative; i<strlen_str1; i++) {
		if(!isdigit(str1[i])) {
			return 0;
		}
		result += (str1[i]-'0')*pow(10,((strlen_str1-1)-i));
	}
	if(negative) {
		result *= -negative;
	}
	return result;
}


void q1(int argc, char *argv[]) {
	int i = con1(argv[1]);
	printf("%d\n",i);
}


int main(int argc, char *argv[]) {
	q2(argc, argv);
	//q1(argc, argv);
	return 0;
}
