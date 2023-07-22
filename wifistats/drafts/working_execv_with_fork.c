#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// testing execv with fork

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
    int  pid;                 // some systems define a pid_t

    switch (pid = fork()) {
    case -1 :
        perror("fork()");     // process creation failed
        exit(1);
        break;

    case 0:                   // new child process
        execv("/usr/bin/sort", newargs);
        break;

    default:                  // original parent process
        sleep(1);
        printf("p:  value of pid=%i\n", pid);
        printf("p:  parent's pid=%i\n", getpid());
        printf("p:  parent's parent pid=%i\n", getppid());
        break;
    }
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    function(); //argv[1]);
    return 0;
}

