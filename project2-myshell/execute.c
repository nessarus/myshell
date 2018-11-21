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


void find_path_execute(SHELLCMD *t)
{
    char *temp=strtok(PATH,":");
    while(temp)
    {
        char temp_path[strlen(temp)+strlen(t->argv[0])];
        strcpy(temp_path,temp);
        strcat(temp_path,"/");
        strcat(temp_path,t->argv[0]);
        //printf("temp_path is %s\n",temp_path);
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

    exit_return_value(t);
  
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
/*            char current_path[MAXPATHLEN];
            getcwd(current_path,sizeof(current_path));
            printf("current_path is %s\n",current_path);
*/
            char previous_directory[MAXPATHLEN];
            getcwd(previous_directory, MAXPATHLEN);
            char *cd_path;
            cd_path = (char*) malloc(0); //strlen(".:..") * sizeof(char));

            //            char *argv_dir = t->argv[1];
            char *end_argv_dir;
            char *argv_dir = strtok_r(t->argv[1],"/", &end_argv_dir);
            while(argv_dir)
            {
                printf("argv_dir is %s\n",argv_dir);

                cd_path = (char*) realloc(cd_path,0); //strlen(".:..") * sizeof(char));
                if(cd_path == NULL)
                {
                    printf("Cannot allocate memory\n");
                    exit( EXIT_FAILURE );
                }

                DIR *dirp;
                dirp = opendir("."); //current_path);
                if(dirp == NULL)
                {
                    perror( "Cannot open directory" );
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
                        printf("dp->d_name is %s\n",dp->d_name);
                        /*
                           static char buffer[strlen(cd_path)+strlen(dp->d_name)+strlen(":")];
                           sprintf(buffer, "%s:%s", cd_path,dp->d_name);
                           cd_path = buffer;
                           */
                        cd_path = realloc(cd_path,(strlen(cd_path)+strlen(dp->d_name)+1)*sizeof(cd_path[0]));
                        if(cd_path == NULL)
                        {
                            printf("Cannot allocate cd_path memory\n");
                            exit( EXIT_FAILURE );
                        }
                        sprintf(cd_path, "%s%s:", cd_path, dp->d_name); 
                    }
                }
                //            temp_cd_path[strlen(temp_cd_path)-1]='\0';
                printf("cd_path new directory is %s\n",cd_path);
                closedir(dirp);




                char *end_cd_path;
                char *temp=strtok_r(cd_path,":",&end_cd_path);
                bool dir_found = false;
                while(temp)
                {
                    if(strcmp(temp,argv_dir)==0)
                    {
                        printf("\n\n");
                        printf("temp is %s\n",temp);

                        chdir(temp);
                        dir_found = true;
                        //                    char temp1[200];
                        //                    strcpy(temp1,current_path);
                        //                    strcat(temp1,"/");
                        //                    strcat(temp1,temp);
                        //                    current_path=temp1;
                        break;
                    }
                    temp = strtok_r(NULL,":",&end_cd_path);
                }
                /*            getcwd(current_path,sizeof(current_path));
                              printf("current_path IS %s\n",current_path);
                              */            
                if(!dir_found)
                {
                    printf("%s: %s: No such file or directory\n", t->argv[0],t->argv[1]);
                    chdir(previous_directory);
                    break;
                }
                argv_dir = strtok_r(NULL,"/",&end_argv_dir);
            }
            free(cd_path);
        }
        return exitstatus;
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

