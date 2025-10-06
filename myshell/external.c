/**
 * @file    external.c
 * @author  Joshua Ng
 * @brief   Execute external commands.
 * @date    2023-08-14
 */

#include "external.h"
#include "globals.h"
#include "filepaths.h"
#include "searchpath.h"
#include "shellscript.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/**
 * @brief Executes a shell command.
 * 
 * @param t     The shell command.
 * @return The exit status.
 */
int external_shellcmd(SHELLCMD *t)
{
    pid_t fpid = fork();
    check_error(fpid);
    int exitstatus = EXIT_SUCCESS;

    if (fpid == 0)
    {
        char *filepath = t->argv[0];
        char *filepath0 = NULL;

        if (strchr(filepath, '/') == NULL)
        {
            filepath0 = searchpath(PATH, filepath);
            if (filepath0 != NULL)
            {
                filepath = filepath0;
            }
        }

        char* filename = get_filename(filepath);
        char* old_argv0 = t->argv[0];
        t->argv[0] = filename;
        execv(filepath, t->argv);
        t->argv[0] = old_argv0;
        char *error_message = strdup(strerror(errno));
        exitstatus = shellscript_shellcmd(t);

        if (exitstatus == EXIT_FAILURE)
        {
            fprintf(stderr, "%s: %s: %s", name0, error_message, t->argv[0]);
        }

        free(filepath0);
        free(filename);
        free(error_message);
        exit(exitstatus);
    }

    int status;                 // Declare a variable to store the child's status
    waitpid(fpid, &status, 0);  // Wait for the child process and get its status 
    exitstatus = WIFEXITED(status) 
        ? WEXITSTATUS(status)   // The child exited normally, update with child's exit status.
        : EXIT_FAILURE;         // The child failed to exit normally.

    return exitstatus;
}
