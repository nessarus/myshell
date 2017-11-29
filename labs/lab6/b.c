#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

void counter(FILE *fp)
{
    char  line[BUFSIZ];
    int   nlines = 0;

    while(fgets(line, sizeof line, fp) != NULL)
    {  
        ++nlines;
    }
    printf("%i\n", nlines);
}

int main(int argc, char *argv[])
{
//  ATTEMPT TO OPEN AND READ FROM PROVIDED FILENAME
    if(argc > 1)
    {
        char *filename = argv[1];
        FILE *fp = fopen(filename, "r");

        if(fp == NULL)
        {
            fprintf(stderr, "cannot open %s\n", filename);
            exit(EXIT_FAILURE);
        }
        counter(fp);
        fclose(fp);
    }
//  NO FILENAME PROVIDED, READ FROM STANDARD-INPUT
    else
    {
        counter(stdin);
    }
    return 0;
} 
