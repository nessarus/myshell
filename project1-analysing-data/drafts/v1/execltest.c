#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void start_sort(char output_txt[], char k[]) {
    char *newargs[] = {
        "sort",
        "-o",
        output_txt,
        "-k",
        k,
        NULL
    };
    execv("/usr/bin/sort",newargs);
}

void function(char fileName[])
{
    char *newargs[] = {
        "sort",
        "-o",
        "e.txt",
        "-k",
        "2",
        "sample-packets.txt",
        NULL
    };
    //char *environ[]={0};
    int pid; // some system define a pid_t

    switch (pid = fork()) {
        case -1:
            perror("fork()"); // process creation failed
            exit(0);
            break;

        case 0:
            execv( "usr/bin/sort", newargs);
            exit(EXIT_FAILURE);
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
    function(argv[1]);
    return 0;
}

