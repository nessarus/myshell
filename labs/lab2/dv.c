/* Display the decimal value of selected characters,
	Written by:
	Date: 13/8/2017
*/
#include <stdio.h>

int main(void)
{
// 	Local Declarations
	char A		= 'A';
	char a 		= 'a';
	char B		= 'B';
	char b		= 'b';
	char Zed	= 'Z';
	char zed	= 'z';
	char zero	= '0';
	char eight	= '8';
	char NL		= '\n';	// newline
	char HT		= '\t'; // horizontal tab
	char VT		= '\v'; // vertical tab
	char SP		= ' ';	// black or space
	char BEL	= '\a'; // alert (bell)
	char dblQuote	= '"';	// double quote
	char backSlash	= '\\';	// backslash itself
	char oneQuote	= '\'';	// single quote itself

//	Statements
	printf("ASCII for char 'A'	is: %d\n", A);
	printf("ASCII for char 'a'	is: %d\n", a);
	printf("ASCII for char 'B'	is: %d\n", B);
	printf("ASCII for char 'b'	is: %d\n", b);
	printf("ASCII for char 'Z'	is: %d\n", Zed);
	printf("ASCII for char 'z'	is: %d\n", zed);
	printf("ASCII for char '0'	is: %d\n", zero);
	printf("ASCII for char '8'	is: %d\n", eight);
	printf("ASCII for char '\\n'	is: %d\n", NL);
	printf("ASCII for char '\\t'	is: %d\n", HT);
	printf("ASCII for char '\\v'	is: %d\n", VT);
	printf("ASCII for char ' '	is: %d\n", SP);
	printf("ASCII for char '\\a'	is: %d\n", BEL);
	printf("ASCII for char '\"'	is: %d\n", dblQuote);
	printf("ASCII for char '\\'	is: %d\n", backSlash);
	printf("ASCII for char '\''	is: %d\n", oneQuote);
	
	return 0;
}	// main
