#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// testing pipe parent to child

int main(void)
{
        int     fd[2]; 
        int     childpid;
        char    string[] = "Hello, world!\n";
        char    readbuffer[80];

        pipe(fd);
        
        if((childpid = fork()) == -1)
        {
                perror("fork");
                exit(1);
        }

        if(childpid == 0)
        {
                /* closes up output side of pipe */
                close(fd[1]);

                /* read in a string from the pipe */
                read(fd[0], readbuffer, sizeof(readbuffer));
                printf("Received string: %s", readbuffer);
                exit(0);
        }
        else
        {
                /* close up input side of pipe */
                close(fd[0]);

                /* send "string" through the output side of pipe */
                write(fd[1], string, (strlen(string)+1));
        }
        
        return(0);
}
