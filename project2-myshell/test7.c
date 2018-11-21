#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

int main(int argc, char*argv[])
{
    FILE *fp=fopen("/Users/minruilu/Desktop/Project2/myshell11/haha","r");
    if (fp!=NULL)
        printf("exist!\n");
    else
        printf("not exist!\n");
    return 0;
}
