#include "myshell.h"

/*
   CITS2002 Project 2 2017
   Name(s):     student-name1 (, student-name2)
   Student number(s):   student-number-1 (, student-number-2)
   Date:        date-of-submission
 */

// -------------------------------------------------------------------

//  THIS FUNCTION SHOULD TRAVERSE THE COMMAND-TREE and EXECUTE THE COMMANDS
//  THAT IT HOLDS, RETURNING THE APPROPRIATE EXIT-STATUS.
//  READ print_shellcmd0() IN globals.c TO SEE HOW TO TRAVERSE THE COMMAND-TREE


char* edit_cd_path(char *cd_path)
{
    cd_path = (char*) realloc(cd_path,sizeof(char)); //strlen(".:..") * sizeof(char));
    if(cd_path == NULL)
    {
        printf("Cannot allocate memory\n");
        exit( EXIT_FAILURE );
    }
    
    const char* current_path = ".";
    DIR *dirp = opendir(current_path);
    if(dirp == NULL)
    {
        perror( argv0 );
        exit(EXIT_FAILURE);
    }
    
    struct dirent *dp;
    char fullpath[MAXPATHLEN];
    while((dp = readdir(dirp)) != NULL)
    {
        struct stat stat_buffer;
        struct stat *pointer = &stat_buffer;
        
        sprintf(fullpath, "%s/%s", ".", dp->d_name); // current_path, dp->d_name);
        
        if(stat(fullpath, pointer) != 0)
        {
            perror( "Cannot open stat" );
            //                        exit( EXIT_FAILURE );
        }
        else if( S_ISDIR( pointer->st_mode ))
        {
            cd_path = realloc(cd_path,(strlen(cd_path)+strlen(dp->d_name)+2)*sizeof(char));
            if(cd_path == NULL)
            {
                perror("Cannot allocate cd_path memory\n");
                exit( EXIT_FAILURE );
            }
            sprintf(cd_path, "%s%s:", cd_path, dp->d_name);
        }
    }
    return cd_path;
}
    



void cd_not_path_execution(SHELLCMD *t)
{
        char previous_directory[MAXPATHLEN];
        getcwd(previous_directory, MAXPATHLEN);
        char *cd_path;
        cd_path = (char*) malloc(0); 
    
        char *end_argv_dir;
        char *argv_dir = strtok_r(t->argv[1],"/", &end_argv_dir);
        while(argv_dir)
        {
            
            cd_path=edit_cd_path(cd_path);
            
            char *end_cd_path;
            char *temp=strtok_r(cd_path,":",&end_cd_path);
            bool dir_found = false;
            while(temp)
            {
                if(strcmp(temp,argv_dir)==0)
                {
                    chdir(temp);
                    dir_found = true;
                    break;
                }
                temp = strtok_r(NULL,":",&end_cd_path);
            }
         
            if(!dir_found)
            {
                printf("%s: %s: No such file or directory\n", t->argv[0],t->argv[1]);
                chdir(previous_directory);
                break;
            }
            argv_dir = strtok_r(NULL,"/",&end_argv_dir);
        }
        free(cd_path);
        return ;
}
int time_execution(SHELLCMD *t)
{
    struct  timeval start;
    struct  timeval end;
    
    unsigned  long diff=0;
    if (t->argc==1)
    {
        printf("\n\nexecution time is 0 millionseconds\n");
        return EXIT_SUCCESS;
    }
        
    gettimeofday(&start,NULL);
    for (int i=0;i<t->argc-1;i++)
    {
        t->argv[i]=strdup(t->argv[i+1]);
    }
    t->argv[t->argc-1]=NULL;
    t->argc--;
    int exitstatus_temp;
    exitstatus_temp=execute_shellcmd(t);
    gettimeofday(&end,NULL);
    diff = 1000000 * (end.tv_sec-start.tv_sec)+ end.tv_usec-start.tv_usec;
    printf("\n\nexecution time is %ld millionseconds\n",diff);
    return exitstatus_temp;
}

void find_path_execute(SHELLCMD *t)
{
    char *temp=strtok(PATH,":");
    while(temp)
    {
        char temp_path[strlen(temp)+strlen(t->argv[0])];
        sprintf(temp_path,"%s/%s",temp,t->argv[0]);
        
        FILE *fp=fopen(temp_path,"r");
        if(fp!=NULL)
        {
            execv(temp_path,t->argv);
        }
        temp = strtok(NULL,":");

    }

    fprintf(stderr, "%s: command  not found\n",t->argv[0]);
    return ;
}

void exit_return_value(SHELLCMD *t)
{
    if(strcmp(t->argv[0],"exit")!=0)
    {
        return;
    }
    if(t->argc<=1)
    {
        exit(previous_exitstatus);
    }
    if(strcmp(t->argv[1],"0")==0)
    {
        exit(EXIT_SUCCESS);
    }
    if(strcmp(t->argv[1],"1")==0)
    {
        exit(EXIT_FAILURE);
    }
    exit(previous_exitstatus);
}

int sequential_execution(SHELLCMD *t)
{
    switch(t->type)
    {
        case CMD_SEMICOLON:
        {
            previous_exitstatus = execute_shellcmd(t->left);
           return execute_shellcmd(t->right);
        }
        case CMD_AND:
        {
            return  execute_shellcmd(t->left) || execute_shellcmd(t->right);
        }
        case CMD_OR:
        {
            return  execute_shellcmd(t->left) && execute_shellcmd(t->right);
        } 
        case CMD_COMMAND:
            break;
        case CMD_SUBSHELL:
        {
            pid_t subshell_pid;
            subshell_pid=fork();
            if(subshell_pid==0)
            {

                exit(execute_shellcmd(t->left));
            }
            else
            {
                wait(&subshell_pid);
                return subshell_pid;
            }
        }
        case CMD_PIPE:
            break;
        case CMD_BACKGROUND:
            break;
    }

    return EXIT_FAILURE;
}

int execute_shellcmd(SHELLCMD *t)
{
    int  exitstatus;
    //int return_value;

    if(t == NULL) {         // hmmmm, that's a problem
        exitstatus  = EXIT_FAILURE;
        return exitstatus;
    }
    else {              // normal, exit commands
        exitstatus  = EXIT_SUCCESS;
    }

    if( t->type!=CMD_COMMAND )
    {
        exitstatus=sequential_execution(t);
        return exitstatus;
    }

    exit_return_value(t);
    
    if (strcmp(t->argv[0],"time")==0)
    {
        exitstatus=time_execution(t);
        return exitstatus;
    }
  
    if(strcmp(t->argv[0],"cd")==0)
    {
        if(t->argc==1)
        {
            chdir(HOME);
        }
        else if(t->argv[1][0]=='/')
        {
            chdir(t->argv[1]);
        }
        else
        {
            cd_not_path_execution(t);
        }
        return exitstatus;
    }

    pid_t fpid;
    switch (fpid=fork()) // fork
    {
        case -1:
        {
            perror( argv0 );
            exit(EXIT_FAILURE);
        }
        
        case 0: // child process
        {
            if(t->infile != NULL)
            {
                int fp = open(t->infile, O_RDONLY);
                if(fp == -1)
                {
                    perror( argv0 );
                    exit(EXIT_FAILURE);
                }

                dup2(fp,STDIN_FILENO); // connect file to stdin
            }
            char *strtemp = strchr(t->argv[0],'/');
            if(strtemp!=NULL)
            {
                exit(execv(t->argv[0],t->argv));
            }

            find_path_execute(t);
            exit(EXIT_FAILURE);
        }
        default: // parent process
        {
            wait(&fpid);
            exitstatus=fpid;
            break;
        }
    }

    return exitstatus;
}

