#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
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
*/

void function(void)
{
    int  pid;                 // some systems define a pid_t

    switch (pid = fork()) {
    case -1 :
        perror("fork()");     // process creation failed
        exit(1);
        break;

    case 0:                   // new child process
        printf("c:  value of pid=%i\n", pid);
        printf("c:  child's pid=%i\n", getpid());
        printf("c:  child's parent pid=%i\n", getppid());
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

