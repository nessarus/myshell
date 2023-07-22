#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char **argv, char **envp)
{
        int fd[2];
        int pid;

        pipe(fd);

        pid = fork();
        if (pid == -1) {
                exit(EXIT_FAILURE);
        }

        if (pid == 0) { /* child */
            close(fd[1]);
            dup2(fd[0],0);
            
                char *exe[]= { "/usr/bin/sort", NULL };
                close(fd[0]);
                execve("/usr/bin/sort", exe, envp);

        }
        else {
            close(fd[0]);
            dup2(fd[1],1);

                char *a[] = { "zilda", "andrew", "bartholomeu", NULL };
                int i;

                close(fd[1]);

                for (i = 0; a[i]; i++)
                        printf("%s\n", a[i]);
        }

        return 0;
}
