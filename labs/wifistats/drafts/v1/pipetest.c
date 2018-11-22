#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    pid_t p;
    int status;
    int fds[2];
    FILE *writeToChild;
    char word[50];

    if (pipe(fds) == -1)
    {
        perror("Error creating pipes");
        exit(EXIT_FAILURE);
    }

    switch (p = fork())
    {
        case 0: //this is the child process
            close(fds[1]); //close the write end of the pipe
            dup2(fds[0], 0);
            close(fds[0]);
            execl("/usr/bin/sort", "sort", (char *) 0);
            fprintf(stderr, "Failed to exec sort\n");
            exit(EXIT_FAILURE);

        case -1: //failure to fork case
            perror("Could not create child");
            exit(EXIT_FAILURE);

        default: //this is the parent process
            close(fds[0]); //close the read end of the pipe
            writeToChild = fdopen(fds[1], "w");
            break;
    }

    if (writeToChild != 0)
    {
        while (fscanf(stdin, "%49s", word) != EOF)
        {
            //the below isn't being printed.  Why?
            fprintf(writeToChild, "%s end of sentence\n", word); 
        }
        fclose(writeToChild);
    }

    wait(&status);

    return 0;
}
