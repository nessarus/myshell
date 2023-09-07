/**
 * @file    pipeline.c
 * @author  Joshua Ng
 * @brief   Put output of command1 as input of command2.
 * @date    2023-09-08
 */

#include "pipeline.h"
#include "globals.h"
#include <unistd.h>
#include <stdlib.h>

/**
 * @brief Describes the read and write file descriptors ends.
 */
enum FILEDESCRIPTOR 
{
    READ_END = 0,
    WRITE_END = 1
};

/**
 * @brief  Pipeline pass output of command1 as input of command2.
 * 
 * @param t     The shell command.
 * @return The exit status of the command.
 */
int pipeline_shellcmd(SHELLCMD *t)
{
    int exitstatus = EXIT_SUCCESS;
    int fd[2];
    pipe(fd);
    pid_t fpid = fork();
    check_error(fpid);

    if (fpid == 0)  // Command 1.
    {
        close(fd[READ_END]);
        dup2(fd[WRITE_END], STDOUT_FILENO);
        check_error(execute_shellcmd(t->left));
        exit(EXIT_FAILURE);
    }

    close(fd[WRITE_END]);
    int status;
    waitpid(fpid, &status, 0);
    exitstatus = WIFEXITED(status) ? WEXITSTATUS(status) : EXIT_FAILURE;

    if (exitstatus != EXIT_SUCCESS) // Command 1 failed.
    {
        close(fd[READ_END]);
        return EXIT_FAILURE;
    }

    fpid = fork();
    check_error(fpid);

    if (fpid == 0) // Command 2.
    {
        dup2(fd[READ_END], STDIN_FILENO);
        check_error(execute_shellcmd(t->right));
        exit(EXIT_FAILURE);
    }

    close(fd[READ_END]);
    waitpid(fpid, &status, 0);
    exitstatus = WIFEXITED(status) ? WEXITSTATUS(status) : EXIT_FAILURE;
    return exitstatus;
}
