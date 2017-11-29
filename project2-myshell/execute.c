#include "myshell.h"

/*
   CITS2002 Project 2 2017
   Name(s):     Minrui Lu (, Joshua Ng)
   Student number(s):   22278526 (, 20163079)
   Date:        3rd-Nov-2017
 */

// -------------------------------------------------------------------

//  THIS FUNCTION SHOULD TRAVERSE THE COMMAND-TREE and EXECUTE THE COMMANDS
//  THAT IT HOLDS, RETURNING THE APPROPRIATE EXIT-STATUS.
//  READ print_shellcmd0() IN globals.c TO SEE HOW TO TRAVERSE THE COMMAND-TREE

int execute_shellcmd(SHELLCMD *t)
{
    int  exitstatus;
    if(t == NULL) {         // hmmmm, that's a problem
        exitstatus  = EXIT_FAILURE;
        return exitstatus;
    }
    else              // normal, exit commands
        exitstatus  = EXIT_SUCCESS;
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO); // save the backup of stdin and stdout
    if(redirection_preparation(t))
        return EXIT_FAILURE;
    if( t->type!=CMD_COMMAND ){
        exitstatus=sequential_execution(t);
        dup2(saved_stdin,STDIN_FILENO);
        dup2(saved_stdout,STDOUT_FILENO);// change input and output node back to default ones
        return exitstatus;
    } // run the sequential exection
    exit_return_value(t); // check the whether command lines include "exit"
    // check if command is "time"
    if (strcmp(t->argv[0],"time")==0){
        exitstatus=time_execution(t);
        return exitstatus;
    }
    //check if command is "cd"
    if(strcmp(t->argv[0],"cd")==0){
        if(t->argc==1)
            chdir(HOME);
        else if(t->argv[1][0]==SLASH_CHAR)
            chdir(t->argv[1]);
        else
            cd_not_path_execution(t);
        return exitstatus;
    }
    pid_t fpid;// step1,execute command
    switch (fpid=fork()){ // fork, creating a child process and parent process
        case -1:{
            perror( argv0 );
            exit(EXIT_FAILURE);
        }
        case 0: // child process
            execute_command(t);
        default:{ // parent process
            waitpid(fpid,&fpid,0);
            exitstatus=fpid;
            dup2(saved_stdin,STDIN_FILENO);
            dup2(saved_stdout,STDOUT_FILENO);
            if (exitstatus)// check whether command is a shell script name or not
                if (access(t->argv[0],0)==0)
                    shell_script_execution(t);
            break;
        }
    }
    return exitstatus;
}

