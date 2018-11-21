#include <stdlib.h>
#include <stdio.h>

char *strcat_array( char dest[], char src[] )
{
    int i = 0;

    while (dest[i] != '\0')
    {
        ++i;
    }

    int j = 0;
    while (src[j] != '\0' )
    {
        dest[ j+i ] = src[ j ];
        ++j;
    }
    dest[ i+j ] = '\0';

    return dest;
}

int main(int argc, char* argv[])
{
    char a[50] = "hello";
    char b[] = " Josh.";

    strcat_array(a,b);

    printf("%s\n",a);
    return 0;
}
