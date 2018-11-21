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

// function designed for step 7
int pipe_execution(SHELLCMD *t)
{
    int fd[2];
    pipe(fd);
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO); //store the backup default STDIN and STDOUT file descriptor
    pid_t pipe_pid;
    switch (pipe_pid = fork()) { // create a child process
        case -1:{
            perror( argv0 );
            exit(EXIT_FAILURE);
            break;
        }
        case 0:{ // child
            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);
            exit(execute_shellcmd(t->left)); // execute the left command--t->left
        }
        default:{ // parent
            wait(&pipe_pid);
            close(fd[1]);
            pid_t pipe2_pid;
            switch(pipe2_pid = fork()) // create another child process to execute the t->right command
            {
                case -1:{
                    perror( argv0 );
                    exit(EXIT_FAILURE);
                    break;
                }
                case 0: {// child 2
                    dup2(fd[0], STDIN_FILENO);
                    exit(execute_shellcmd(t->right));
                }
                default:{ // parent
                    wait(&pipe2_pid);
                    close(fd[0]);
                    dup2(saved_stdin,STDIN_FILENO);
                    dup2(saved_stdout,STDOUT_FILENO);// change the default STDIN and STDOUT file descriptor back
                    return pipe2_pid;
                }
            }
        }
    }
}
// function used for step3 cd part, editing the paths used in cd
char* edit_cd_path(char *cd_path)
{
    cd_path = (char*) realloc(cd_path,sizeof(char)); // initialise cd_path
    if(cd_path == NULL){
        printf("Cannot allocate memory\n");
        exit( EXIT_FAILURE );
    }
    const char* current_path = "."; // In linux shell, "." means the current directory
    DIR *dirp = opendir(current_path);
    if(dirp == NULL){
        perror( "Cannot open directory" );
        exit(EXIT_FAILURE);
    }
    struct dirent *dp;
    char fullpath[MAXPATHLEN];
    while((dp = readdir(dirp)) != NULL){
        struct stat stat_buffer;
        struct stat *pointer = &stat_buffer;
        sprintf(fullpath, "%s/%s", ".", dp->d_name); // current_path, dp->d_name);
        if(stat(fullpath, pointer) != 0){
            perror( "Cannot open stat" );
            exit( EXIT_FAILURE );
        }
        else if( S_ISDIR( pointer->st_mode )){
            cd_path = realloc(cd_path,(strlen(cd_path)+strlen(dp->d_name)+2)*sizeof(char));
            if(cd_path == NULL){
                printf("Cannot allocate cd_path memory\n");
                exit( EXIT_FAILURE );
            }
            sprintf(cd_path, "%s%s:", cd_path, dp->d_name);
        }// attach new directory name to cd_paths
    }
    return cd_path;
}
// when we just type the directory name following the "cd" command, we look up the directory name in current paths to execute it
void cd_not_path_execution(SHELLCMD *t)
{
        char previous_directory[MAXPATHLEN];
        getcwd(previous_directory, MAXPATHLEN);
        char *cd_path;
        cd_path = (char*) malloc(0); //initialise the cd_path
        char *end_argv_dir;
        char *argv_dir = strtok_r(t->argv[1],"/", &end_argv_dir);
        while(argv_dir){
            cd_path=edit_cd_path(cd_path);//get the cd_path we want
            char *end_cd_path;
            char *temp=strtok_r(cd_path,":",&end_cd_path);// get each possible path once a time
            bool dir_found = false;
            while(temp){
                if(strcmp(temp,argv_dir)==0){
                    chdir(temp);
                    dir_found = true;
                    break;
                }// if the path matches, then change the directory to it
                temp = strtok_r(NULL,":",&end_cd_path);// if path does not match, then change to next candidate path
            }
            if(!dir_found){
                printf("%s: %s: No such file or directory\n", t->argv[0],t->argv[1]);
                chdir(previous_directory);
                break;
            }
            argv_dir = strtok_r(NULL,"/",&end_argv_dir);
        }
        free(cd_path);
        return;
}
// if t->argv[0] is time, then time the following commands
int time_execution(SHELLCMD *t)
{
    struct  timeval start;
    struct  timeval end;
    unsigned  long diff=0; //diff is difference of end time minus start time
    if (t->argc==1){
        printf("\n 0 msec\n");
        return EXIT_SUCCESS;
    }
    gettimeofday(&start,NULL);
    for (int i=0;i<t->argc-1;i++)//change the t->argv[]
        t->argv[i]=strdup(t->argv[i+1]);
    t->argv[t->argc-1]=NULL;
    t->argc--;
    int exitstatus_temp;
    exitstatus_temp=execute_shellcmd(t);// execute the commands following "time"
    gettimeofday(&end,NULL);
    diff =(end.tv_usec-start.tv_usec)/1000;// change the time into millionseconds
    printf("\n %ld msec\n",diff);
    return exitstatus_temp;
}
// when we type in the command name, we look up the commands in default paths
void find_path_execute(SHELLCMD *t)
{
    char *temp=strtok(PATH,":");
    while(temp){
        char temp_path[strlen(temp)+strlen(t->argv[0])];
        sprintf(temp_path,"%s/%s",temp,t->argv[0]);
        if (access(temp_path,0)==0)
            execv(temp_path,t->argv);
        temp = strtok(NULL,":");
    }
    if (access(t->argv[0],0)!=0)
        fprintf(stderr, "%s: command not found\n",t->argv[0]);
    return ;
}
// check whether we are typing in exit
void exit_return_value(SHELLCMD *t)
{
    if(strcmp(t->argv[0],"exit")!=0)
        return;
    if(t->argc<=1)
        exit(previous_exitstatus);
    if(strcmp(t->argv[1],"0")==0)
        exit(EXIT_SUCCESS);
    if(strcmp(t->argv[1],"1")==0)
        exit(EXIT_FAILURE);
    exit(previous_exitstatus);// if the command after exit is not 0 or 1
}

void terminate_child(int pid)
{
    printf("Background PID ended: %d\n", getpid());
}

// different execution for different t->type
int sequential_execution(SHELLCMD *t)
{
    switch(t->type){
        case CMD_SEMICOLON:{
            previous_exitstatus = execute_shellcmd(t->left);
            return execute_shellcmd(t->right);
        }
        case CMD_AND:{
            return  execute_shellcmd(t->left) || execute_shellcmd(t->right);
        }
        case CMD_OR:{
            return  execute_shellcmd(t->left) && execute_shellcmd(t->right);
        } 
        case CMD_COMMAND:
            break;
        case CMD_SUBSHELL:{// create a new fork to execute the command in subshell
            pid_t subshell_pid;
            subshell_pid=fork();
            if (subshell_pid==-1){
                perror("create subshell child process failed\n");
                exit(EXIT_FAILURE);
            }
            else if(subshell_pid==0)// child process
                exit(execute_shellcmd(t->left));
            else{ // parent process
                wait(&subshell_pid);
                return subshell_pid;
            }
        }
        case CMD_PIPE:{
            return pipe_execution(t);
            break;
        }
        case CMD_BACKGROUND:
        {
            signal(SIGTERM, (void (*)(int))terminate_child);
            pid_t background_pid;
            if(t->left->type == CMD_SEMICOLON)
            {
                previous_exitstatus = execute_shellcmd(t->left->left);
                switch(background_pid = fork())
                {
                    case -1:
                    {
                        perror("create background child process failed\n");
                        exit(EXIT_FAILURE);
                    }
                    case 0:
                    {
                        printf("Started %d\n", getpid());
                        int exitstatus = execute_shellcmd(t->left->right);
                        kill(getpid(),SIGTERM);
                        exit(exitstatus);
                    }
                    default:
                    {
                        return execute_shellcmd(t->right);
                    }
                }
            }
            else
            {
                switch(background_pid = fork())
                {
                    case -1:
                    {
                        perror("create background child process failed\n");
                        exit(EXIT_FAILURE);
                    }
                    case 0:
                    {
                        printf("Started %d\n", getpid());
                        int exitstatus = execute_shellcmd(t->left);
                        kill(getpid(),SIGTERM);
                        exit(exitstatus);
                    }
                    default:
                    {
                        return execute_shellcmd(t->right);
                    }
                }
            }
            break;
        }
    }
    return EXIT_FAILURE;
}
//redirection preparation required in step 6
void redirection_preparation(SHELLCMD *t)
{
    if(t->outfile != NULL){// outfile operation
        int fd;
        if(t->append==false)
            fd = open(t->outfile,O_CREAT|O_RDWR|O_TRUNC); //> output
        else
            fd = open(t->outfile,O_CREAT|O_RDWR|O_APPEND);// >> output
        if(fd==-1)
            perror("\n ERROR: fail to create file");
        else
            dup2(fd,STDOUT_FILENO);
    }
    if(t->infile != NULL){
        int fp = open(t->infile, O_RDONLY);
        if(fp == -1)
        {
            perror( argv0 );
        }
        else
        {
           dup2(fp,STDIN_FILENO); 
        }
    }
    return;
}
//execute the commands typed in
void execute_command(SHELLCMD *t)
{
    char *strtemp = strchr(t->argv[0],'/');
    if(strtemp!=NULL)
        exit(execv(t->argv[0],t->argv));
    find_path_execute(t);
    exit(EXIT_FAILURE);
    return;
}
// execute shell script in step8
void shell_script_execution(SHELLCMD *t)
{
    FILE *file=fopen(t->argv[0],"r");
    int saved_stdin = dup(STDIN_FILENO); // store the default STDIN file descriptor
    char *arguments[2];
    arguments[0]="myshell";
    arguments[1]=NULL;
    pid_t script_pid =fork();
    if(script_pid == -1) {                             // process creation failed
        printf("fork() failed\n");
        exit(EXIT_FAILURE);
    }
    else if(script_pid==0){   // child process
        dup2(fileno(file),STDIN_FILENO);  // change the STDIN to the file pointer
        execv(directory,arguments);
        printf("execv failed!\n"); // if execv fails, then printf can run
        exit(EXIT_FAILURE);
    }
    else{    //parent process
        wait(&script_pid);
        fclose(file);
        dup2(saved_stdin,STDIN_FILENO);  // change the input file descriptor to the standard one
    }
    return;
}

int execute_shellcmd(SHELLCMD *t)
{
    int  exitstatus;
    if(t == NULL) {         // hmmmm, that's a problem
        exitstatus  = EXIT_FAILURE;
        return exitstatus;
    }
    else {              // normal, exit commands
        exitstatus  = EXIT_SUCCESS;
    }
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO); // save the backup of stdin and stdout
    redirection_preparation(t);
    if( t->type!=CMD_COMMAND ){
        exitstatus=sequential_execution(t);
        dup2(saved_stdin,STDIN_FILENO);
        dup2(saved_stdout,STDOUT_FILENO);
        return exitstatus;
    } // run the sequential exection
    exit_return_value(t); // check the whether command lines include "exit"
    if (strcmp(t->argv[0],"time")==0){
        exitstatus=time_execution(t);
        return exitstatus;
    }// check if command is "time"
    if(strcmp(t->argv[0],"cd")==0){
        if(t->argc==1)
            chdir(HOME);
        else if(t->argv[1][0]=='/')
            chdir(t->argv[1]);
        else
            cd_not_path_execution(t);
        return exitstatus;
    }//check if command is "cd"
    pid_t fpid;
    switch (fpid=fork()){ // fork
        case -1:{
            perror( argv0 );
            exit(EXIT_FAILURE);
        }
        case 0: {// child process
        execute_command(t);
        }
        default:{ // parent process
            waitpid(fpid, &fpid, 0);
            exitstatus=fpid;
            dup2(saved_stdin,STDIN_FILENO);
            dup2(saved_stdout,STDOUT_FILENO);
            if (exitstatus)
                if (access(t->argv[0],0)==0)
                    shell_script_execution(t);
            break;
        }
    }
    return exitstatus;
}

