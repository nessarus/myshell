#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

int main(int argc, char*argv[])
{
    //int semicolon_left_execution =false;
    bool semicolon_left_execution=false;
    bool *p=&semicolon_left_execution;
   
//    int n;
//    int fd[2];
        pid_t pid;
//    char line[BUFSIZ];
//
//    if(pipe(fd)<0)
//        perror("pipe error");
    if((pid=fork())<0)
    {
        perror("fork error");
    }
    else if (pid>0)
    {
//        close(fd[0]);
//        write(fd[1],p,10);
        *p=true;
         printf("%i\n",semicolon_left_execution);
        exit(EXIT_SUCCESS);
    }
    else
    {
        wait(&pid);
        printf("%i\n",semicolon_left_execution);
    }
    return 0;
}
