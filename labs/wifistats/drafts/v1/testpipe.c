#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
    int     fd[2];
    pid_t   childpid;
    //    char    readbuffer[80];

    //    FILE *pipe_fp;
    pipe(fd);
    //    char line[BUFSIZ];

    if((childpid = fork()) == -1)
    {
        perror("fork");
        exit(1);
    }

    if(childpid == 0)
    {

        char *newargs[] = {
            "sort",
            "-k2",
            "-o",
            "s.txt",
            "sample-packets.txt",
            NULL
        };

        execv("/usr/bin/sort", newargs );
        exit(0);
    }
    else
    {
        /* Child process closes up input side of pipe */
        close(fd[0]);

        /* Send "string" through the output side of pipe */
        write(fd[1], "hello", (strlen("hello")+1));
        exit(0);                                
    }
    sleep(1);
    fflush(stdout);
    return(0);
}
