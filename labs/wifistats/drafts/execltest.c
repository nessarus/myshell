#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void function(char fileName[])
{
/*    char fileN[100] = "-o f.txt -k 2";
    strcat(fileN, fileName);
    char *newargs[] = {
        fileN,
        NULL
    };*/
    char *environ[]={0};
    int pid; // some system define a pid_t

    switch (pid = fork()) {
        case -1:
            perror("fork()"); // process creation failed
            exit(0);
            break;

        case 0:
            execl( "usr/bin/sort", "usr/bin/sort", "-o f.txt", "-k", "2", fileName, environ);
            exit(EXIT_FAILURE);
            break;

        default:                  // original parent process
            sleep(2);
            printf("p:  value of pid=%i\n", pid);
            printf("p:  parent's pid=%i\n", getpid());
            printf("p:  parent's parent pid=%i\n", getppid());
            break;
    }
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    function(argv[1]);
    return 0;
}

