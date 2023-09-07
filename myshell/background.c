/**
 * @file    background.c
 * @author  Joshua Ng
 * @brief   Executes shell commands asynchronously.
 * @date    2023-09-08
 */

#include "background.h"
#include "globals.h"
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

/**
 * @brief Get the set of child background pids.
 * 
 * @return A static set data structure.
 */
static struct SET* get_background_pids(void)
{
    static struct SET* background_pids = NULL;

    if (background_pids == NULL)
    {
        background_pids = set_create();
    }

    return background_pids;
}

/**
 * @brief   Handler for when child terminate signal.
 * 
 * @param signum    The terminate signal enum.  
 */
static void handler_child_final(int signum)
{
    pid_t pid;
    int status;
    struct SET* background_pids = get_background_pids();

    // Loop through all child processes that have changed their state.
    // WNOHANG: Return immediately if no child has changed its state.
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)   
    {   
        if (!set_contains(background_pids, pid))
        {
            continue;
        }

        int exitstatus = EXIT_FAILURE;

        if (WIFEXITED(status)) 
        {
            exitstatus = WEXITSTATUS(status);
        } 
        else if (WIFSIGNALED(status)) 
        {
            printf("Child %d killed by signal %d\n", pid, WTERMSIG(status));
            continue;
        }

        printf("Child %d exited with status %d\n", pid, exitstatus);
        set_remove(background_pids, pid);

        if (set_size(background_pids) == 0)
        {
            signal(SIGCHLD, SIG_DFL);
            break;
        }
    }
}

/**
 * @brief Handles a terminate child signal.
 * 
 * @param signum    The terminate signal enum.
 */
static void handle_child_terminate(int signum)
{
    exit(EXIT_SUCCESS);
}

/**
 * @brief Executes a background shell command.
 * 
 * @param t     The shell command.
 * @return The exit status.
 */
int background_shellcmd(SHELLCMD *t)
{
    signal(SIGCHLD, handler_child_final);
    pid_t fpid = fork();
    check_error(fpid);

    if (fpid == 0)                  // Child process
    {
        signal(SIGTERM, handle_child_terminate);
        int exitstatus = EXIT_SUCCESS;

        if (t != NULL)
        {
            exitstatus = execute_shellcmd(t);
        }

        exit(exitstatus);
    }

    struct SET* background_pids = get_background_pids();
    set_insert(background_pids, fpid);
    return EXIT_SUCCESS;
}

/**
 * @brief Handles terminating all background processes.
 */
void background_exit(void)
{
    struct SET* background_pids = get_background_pids();
    struct SET_ITERATOR* it = set_iterator_create(background_pids);

    while (set_iterator_has_next(it))
    {
        int pid = set_iterator_get(it);
        kill(SIGTERM, pid);
        set_iterator_next(it);
    }

    set_free(background_pids);
}