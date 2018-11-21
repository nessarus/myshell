#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// testing sending file through pipe

void function(char in_filename[])
{
    int     fd[2]; 
    int     pid;
    char    line[BUFSIZ];
    FILE    *fp_in  = fopen(in_filename,  "r");

    pipe(fd); //pipe

    switch (pid = fork()) { //run fork, splits process
        case -1 :
            perror("fork");
            exit(1);

        case 0: //child
            close(fd[1]); //close piping back to parent
            dup2(fd[0],0); //connect pipe to child 

            char *newarg[] = {"sort", NULL};
            execv("/usr/bin/sort", newarg);
            exit(20);
            break;

        default: //parent
            sleep(1);
            close(fd[0]); //stop child piping back
            dup2(fd[1],1); //connect pipe to child

            if(fp_in != NULL)
            {
                while( fgets(line, sizeof line, fp_in) != NULL)
                {  
                    // send line through the pipe
                    printf("%s", line);
                }
            }
            break;
    }

    if(fp_in != NULL) // close files
        fclose(fp_in);
}

int main(void)
{
    function("sample-packets.txt");
    return 0;
}
