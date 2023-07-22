#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void function(void) //char fileName[])
{
    char fileName[] = "s.txt";
    char *newargs[] = {
        "sort",
        "-o", 
        fileName, 
        "-k", 
        "2", 
        "sample-packets.txt",
        NULL
    };
    execv("/usr/bin/sort", newargs);
}

int main(int argc, char *argv[])
{
    function(); //argv[1]);
    return 0;
}

