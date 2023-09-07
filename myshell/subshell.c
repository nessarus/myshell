/**
 * @file    subshell.c
 * @author  Joshua Ng
 * @brief   Handles subshell command.
 * @date    2023-08-26
 */

#include "subshell.h"
#include "myshell.h"
#include "globals.h"
#include <unistd.h>
#include <stdlib.h>

/**
 * @brief Handles to subshell command.
 * 
 * @param t     The shellcmd to handle.
 * @return The exitstatus of the operation.
 */
int subshell_shellcmd(SHELLCMD *t)
{
    int exitstatus = EXIT_SUCCESS;
    pid_t pid;
    pid = fork();

    switch (pid)
    {
    case -1:
        check_error(pid);
        break;
    case 0:                             // child process
        exit(execute_shellcmd(t->left));
        break;
    default:                            // parent process
    {
        int status;     // Declare a variable to store the exit status
        waitpid(pid, &status, 0);       // Wait for the child.

        exitstatus = WIFEXITED(status)  // True if child exited normally.
            ? WEXITSTATUS(status)       // Update with child exitstatus.
            : EXIT_FAILURE;             // Child exited failure.
        break;
    }
    }

    return exitstatus;
}
