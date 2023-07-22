#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

void forktest(void)
{
    int pid; // some system define a pid_t
/*    char *newargs[] = {
        "sort",
        "-k", 
        "2", 
        NULL
    }; 
*/
    int fd[2];
    pipe(fd);

    switch (pid = fork()) {
        case -1:
            perror("fork");
            exit(1);
            break;
            
        case 0:
            close(0);
            dup(fd[0]);

            execlp("usr/bin/sort", "sort", NULL);
            exit(EXIT_FAILURE);
            break;

        default:                  // original parent process
            close(fd[0]);

            FILE   *dict;
            char   line[BUFSIZ];

            dict = fopen("sample-packets.txt", "r");
            if(dict == NULL) {
                printf( "cannot open dictionary '%s'\n", "sample-packets.txt");
                exit(EXIT_FAILURE);
            }

            while( fgets(line, sizeof line, dict) != NULL )    
            {
                write(fd[1], line, (strlen(line)+1));
            }
            fclose(dict);
            break;
    }
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    forktest();
    return 0;
}

