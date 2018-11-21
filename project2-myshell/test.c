#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

int main (int argc, char*argv[])
{
    pid_t fpid;
    fpid=fork();
    
    if(fpid==0)
    {
        //execute_preparation(t);
        //printf("argv0 is %s\n",argv0);
        //printf("argv[0] is %s\n",t->argv[0]);
        printf("123\n");
//        char ** execv_str=NULL;
//        execv_str[0]="/usr/bin/cal";
//        execv_str[1]="-y";
//        execv_str[2]=NULL;
        
        char *execv_str[argc];
        execv_str[0]="/usr/bin/cal",
        execv_str[1]="-y";
        execv_str[2]=NULL;
        printf("%s\n",execv_str[0]);
        execv("/usr/bin/cal",execv_str);
        exit(EXIT_FAILURE);
    }
    else
    {
        wait(&fpid);
    }
    return 0;
}

