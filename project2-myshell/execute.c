#include "myshell.h"

/*
   CITS2002 Project 2 2017
   Name(s):		student-name1 (, student-name2)
   Student number(s):	student-number-1 (, student-number-2)
   Date:		date-of-submission
 */

// -------------------------------------------------------------------

//  THIS FUNCTION SHOULD TRAVERSE THE COMMAND-TREE and EXECUTE THE COMMANDS
//  THAT IT HOLDS, RETURNING THE APPROPRIATE EXIT-STATUS.
//  READ print_shellcmd0() IN globals.c TO SEE HOW TO TRAVERSE THE COMMAND-TREE


void find_path_execute(SHELLCMD *t)
{
    char *temp=strtok(PATH,":");
    while(temp)
    {
        char *temp_path;
        temp_path=strdup(temp);
        strcat(temp_path,"/");
        strcat(temp_path,t->argv[0]);
        printf("temp_path is %s\n",temp_path);
        FILE *fp=fopen(temp_path,"r");
        if(fp!=NULL)
        {
            execv(temp_path,t->argv);
        }
        temp = strtok(NULL,":");
        
    }
    return ;
}


int execute_shellcmd(SHELLCMD *t)
{
    int  exitstatus;

    if(t == NULL) {			// hmmmm, that's a problem
	exitstatus	= EXIT_FAILURE;
    }
    else {				// normal, exit commands
	exitstatus	= EXIT_SUCCESS;
    }
   
    if(strcmp(t->argv[0],"exit")==0){
        if(t->argc<=1) {
            return 3;
        }
        if(strcmp(t->argv[1],"0")==0) {
            exit(EXIT_SUCCESS);
        }
        if(strcmp(t->argv[1],"1")==0) {
            exit(EXIT_FAILURE);
        }
        return 3;
    }


    pid_t fpid;
    fpid=fork();
    
    if(fpid==0)// child process
    {
        char * strtemp;
        strtemp=strchr(t->argv[0],'/');
        if(strtemp!=NULL)
        {
           execv(t->argv[0],t->argv);
        }

        find_path_execute(t);
        exit(EXIT_FAILURE);
    }
    else // parent process
    {
        wait(&fpid);
    }
    
    

    return exitstatus;
}
