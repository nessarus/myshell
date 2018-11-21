#include "myshell.h"
/*
 CITS2002 Project 2 2017
 Name(s):     Minrui Lu (, Joshua Ng)
 Student number(s):   22278526 (, 20163079)
 Date:        3rd-Nov-2017
 */

//step1: execute the commands typed in
void execute_command(SHELLCMD *t)
{
    char *strtemp = strchr(t->argv[0],SLASH_CHAR);
    if(strtemp!=NULL)// if argv[0] is a path
        exit(execv(t->argv[0],t->argv));
    find_path_execute(t);// if argv[0] is not a path
    exit(EXIT_FAILURE);
    return;
}
//step2: when we type in the command name, we look up the commands in default paths
void find_path_execute(SHELLCMD *t)
{
    char *temp=strtok(PATH,":");// breaks up PATH by ":" tokens i.e. abc:efg:hij
    while(temp){ 
        char temp_path[strlen(temp)+strlen(t->argv[0])];
        sprintf(temp_path,"%s/%s",temp,t->argv[0]);
        if (access(temp_path,0)==0) 
            execv(temp_path,t->argv);   // execute, path found success
        temp = strtok(NULL,":");
    }
    if (access(t->argv[0],0)!=0)// check argv[0] is a shell script or not
        fprintf(stderr, "%s: command not found\n",t->argv[0]); //path find unsuccessful
    return ;
}
//step3:
//exit:
void exit_return_value(SHELLCMD *t)
{
    if(strcmp(t->argv[0],"exit")!=0)
        return;
    kill_child(); // kill all background child processes
    if(t->argc<=1)
        exit(previous_exitstatus); 
    if(strcmp(t->argv[1],"0")==0)
        exit(EXIT_SUCCESS);
    if(strcmp(t->argv[1],"1")==0)
        exit(EXIT_FAILURE);
    exit(previous_exitstatus);// if the command after exit is not 0 or 1
}
//cd:
//editing the paths used in cd
char* edit_cd_path(char *cd_path)
{
    cd_path = (char*) realloc(cd_path,sizeof(char)); 
    if(cd_path == NULL){
        printf("Cannot allocate memory\n");
        exit( EXIT_FAILURE );
    }
    const char* current_path = "."; 
    DIR *dirp = opendir(current_path); //open current directory
    if(dirp == NULL){
        perror( "Cannot open directory" );
        exit(EXIT_FAILURE);
    }
    struct dirent *dp;
    char fullpath[MAXPATHLEN]; //builds cd_path like ./abc:./efg:./hij 
    while((dp = readdir(dirp)) != NULL){
        struct stat stat_buffer;
        struct stat *pointer = &stat_buffer;
        sprintf(fullpath, "%s/%s", ".", dp->d_name); 
        if(stat(fullpath, pointer) != 0){ //file can't be inspected
            continue;
        }
        else if( S_ISDIR( pointer->st_mode )){//check whether the name is a directory name
            cd_path = realloc(cd_path,(strlen(cd_path)+strlen(dp->d_name)+2)*sizeof(char));
            if(cd_path == NULL){
                printf("Cannot allocate cd_path memory\n");
                exit( EXIT_FAILURE );
            }
            sprintf(cd_path, "%s%s:", cd_path, dp->d_name);//successful new entry
        }
    }
    return cd_path;
}
// change directory when given a directory name without a path 
void cd_not_path_execution(SHELLCMD *t)
{
    char previous_directory[MAXPATHLEN];
    getcwd(previous_directory, MAXPATHLEN);//store current directory in case cd fails
    char *cd_path;
    cd_path = (char*) malloc(0); 
    char *end_argv_dir;
    char *argv_dir = strtok_r(t->argv[1],"/", &end_argv_dir);//breaks up entries like abc/efg/hij
    while(argv_dir){
        cd_path=edit_cd_path(cd_path);
        char *end_cd_path;
        char *temp=strtok_r(cd_path,":",&end_cd_path);//breaks up cd_path .:..:abc:efg:hij
        bool dir_found = false;
        while(temp){
            if(strcmp(temp,argv_dir)==0){
                chdir(temp);                //successful cd change
                dir_found = true;
                break;
            }
            temp = strtok_r(NULL,":",&end_cd_path);
        }
        if(!dir_found){                     //unsuccessful cd change
            printf("%s: %s: No such file or directory\n", t->argv[0],t->argv[1]);
            chdir(previous_directory);
            break;
        }
        argv_dir = strtok_r(NULL,"/",&end_argv_dir);
    }
    free(cd_path);
    return;
}
//time:
int time_execution(SHELLCMD *t)
{
    struct  timeval start;
    struct  timeval end;
    unsigned long diff=0;//diff is difference of end time minus start time
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
    exitstatus_temp=execute_shellcmd(t);//execute the commands following "time"
    gettimeofday(&end,NULL);
    diff =(end.tv_usec-start.tv_usec)/1000;// change the time into millionseconds
    printf("\n %ld msec\n",diff);
    return exitstatus_temp;
}
//step4 to step 9
int sequential_execution(SHELLCMD *t)
{
    switch(t->type){
        case CMD_SEMICOLON:{
            previous_exitstatus = execute_shellcmd(t->left);
            return execute_shellcmd(t->right);
        }
        case CMD_AND:{
            return  execute_shellcmd(t->left) || execute_shellcmd(t->right);
        }//execute_shellcmd returns 0 when execute successfully, so here we use OR logic.
        case CMD_OR:{
            return  execute_shellcmd(t->left) && execute_shellcmd(t->right);
        }//execute_shellcmd returns 0 when execute successfully, so here we use AND logic.
        case CMD_COMMAND:
            break;//see execute_shellcmd
        case CMD_SUBSHELL:{//create a new fork to execute the command in subshell
            pid_t subshell_pid;
            subshell_pid=fork();
            if (subshell_pid==-1){
                perror("create subshell child process failed\n");
                exit(EXIT_FAILURE);
            }
            else if(subshell_pid==0)// child process
                exit(execute_shellcmd(t->left));
            else{ //parent process
                wait(&subshell_pid);
                return subshell_pid;
            }
        }
        case CMD_PIPE:{
            return pipe_execution(t);
            break;
        }
        case CMD_BACKGROUND:{
            return background_execution(t);
            break;
        }
    }
    return EXIT_FAILURE;//ensure the program can exit if no sequential situations match
}
//step 6:
int redirection_preparation(SHELLCMD *t)
{
    if(t->outfile != NULL){//outfile operation
        int fd;
        if(t->append==false)
            fd = open(t->outfile,O_CREAT|O_RDWR|O_TRUNC);//> output
        else
            fd = open(t->outfile,O_CREAT|O_RDWR|O_APPEND);//>> output
        if(fd==-1)
            perror("\n ERROR: fail to create file");
        else
            dup2(fd,STDOUT_FILENO);//change the output node to the file descriptor of open file
    }
    if(t->infile != NULL){
        int fp = open(t->infile, O_RDONLY);
        if(fp == -1){
            perror("Open infile failed!\n");
            return EXIT_FAILURE;
        }
        else
            dup2(fp,STDIN_FILENO);//change the input node to the file descriptor of open file
    }
    return EXIT_SUCCESS;
}
//step 7:
int pipe_execution(SHELLCMD *t)
{
    int fd[2];
    pipe(fd);
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO); //store the backup default STDIN and STDOUT file descriptor
    pid_t pipe_pid;
    switch (pipe_pid = fork()) { //create a child process
        case -1:{
            perror( argv0 );
            exit(EXIT_FAILURE);
            break;
        }
        case 0:{ //left child
            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);//link output to pipe
            exit(execute_shellcmd(t->left));
        }
        default:{ //parent
            wait(&pipe_pid);
            close(fd[1]);
            pid_t pipe2_pid;
            switch(pipe2_pid = fork()) //create another child process to execute the t->right command
            {
                case -1:{
                    perror( argv0 );
                    exit(EXIT_FAILURE);
                    break;
                }
                case 0: { //right child
                    dup2(fd[0], STDIN_FILENO);//link input to pipe
                    exit(execute_shellcmd(t->right));
                }
                default:{ //parent
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
//step 8:
void shell_script_execution(SHELLCMD *t)
{
    FILE *file=fopen(t->argv[0],"r");
    int saved_stdin = dup(STDIN_FILENO); //store the default STDIN file descriptor
    char *arguments[2];
    arguments[0]=argv0;
    arguments[1]=NULL;
    pid_t script_pid =fork();
    if(script_pid == -1) { //process creation failed
        printf("fork() failed\n");
        exit(EXIT_FAILURE);
    }
    else if(script_pid==0){ //child process
        dup2(fileno(file),STDIN_FILENO);//change the STDIN to the file pointer
        execv(directory,arguments);//directory is a global variable
        printf("execv failed!\n"); //if execv fails, then printf can run
        exit(EXIT_FAILURE);
    }
    else{ //parent process
        wait(&script_pid);
        fclose(file);
        dup2(saved_stdin,STDIN_FILENO);  //change the input file descriptor to the standard one
    }
    return;
}
//step 9:
//a function used in signal function
void terminate_child(int pid)
{
    printf("Background PID ended: %d\n", getpid());
}

int background_execution(SHELLCMD *t)
{
    if(t->left==NULL){
        fprintf(stderr,"ERROR:No commands before \"&\"!\n");
        return (EXIT_FAILURE);
    }
    signal(SIGTERM, (void (*)(int))terminate_child);
    pid_t background_pid;
    if(t->left->type == CMD_SEMICOLON){// check whether on the left of "&" there are any ";"
        previous_exitstatus = execute_shellcmd(t->left->left);
        switch(background_pid = fork()){
            case -1:{
                perror("create background child process failed\n");
                exit(EXIT_FAILURE);
            }
            case 0:{ //background process, aka child process
                printf("\nStarted %d\n", getpid());
                push_pid(getpid());//copy the pid to stack
                execute_shellcmd(t->left->right);
                kill(getpid(),SIGTERM);//send a signal
                pop_pid(getpid());// eliminate the pid before exit
                exit(EXIT_SUCCESS);
            }
            default: // parent process
                return execute_shellcmd(t->right);
        }
    }
    else{ // if there are no semicolons before ampersand
        switch(background_pid = fork()){
            case -1:{
                perror("create background child process failed\n");
                exit(EXIT_FAILURE);
            }
            case 0:{
                printf("Started %d\n", getpid());
                execute_shellcmd(t->left);
                kill(getpid(),SIGTERM);
                exit(EXIT_SUCCESS);
            }
            default:
                return execute_shellcmd(t->right);
        }
    }
}
//add new child process id to stack
void push_pid( pid_t newpid)
{
    CHILDPIDS *new = malloc( sizeof(CHILDPIDS) );
    if(new == NULL) {
        perror( __func__ );
        exit(EXIT_FAILURE);
    }
    new->pid    = newpid;
    new->next   = cpids;
    cpids       = new;
}
//eliminate child process id in stack
void pop_pid(pid_t pid)
{
    if(cpids == NULL) {
        fprintf(stderr, "attempt to pop from an empty stack\n");
        exit(EXIT_FAILURE);
    }
    CHILDPIDS *old = cpids;
    if(old->pid == pid){
        cpids = cpids->next;
        free(old);
        return;
    }
    CHILDPIDS *before_old = old;
    while(before_old->next->pid != pid) {
        before_old = before_old->next;
        if(before_old == NULL)
            return;
    }
    old = before_old->next;
    before_old->next = before_old->next->next;
    free(old);
}
// kill all child background processes
void kill_child(void)
{
    CHILDPIDS *c=cpids;
    while(c!=NULL){
        int retval= kill(c->pid,SIGKILL);
        if(retval){
            perror("kill child background process failed!\n");
            exit(EXIT_FAILURE);
        }
        c=c->next;
    }
    return;
}
