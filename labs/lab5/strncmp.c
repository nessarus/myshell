#include <stdio.h>
#include <stdlib.h>

int strncmp_array( char *dest, char *src , int n)
{
    int i = 0;

    while( src[i] != '\0' && dest[i] != '\0' && i!=n )
    {
        if(src[i] != dest[i])
        {
            return src[i] - dest[i];
        }
        i++;
    }
    return src[i] - dest[i];
}

int main(int argc, char* argv[]) 
{
    char a[] = "hello";
    char b[] = "hex2as;dfkljs";
    char c[] = "helasdf";

    int d = strncmp_array( a, b, 2);
    int e = strncmp_array(a,c, 2);

    printf("%d %d\n", d,e);
    return 0;
}
