/**
 * @file    shellscript.c
 * @author  your name (you@domain.com)
 * @brief   Handles a shell script, a file containing more myshell command.
 * @date    2023-09-04
 */

#if defined(__linux__)
    char *strdup(const char *str);
#endif

#include "shellscript.h"
#include "globals.h"
#include "filepaths.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Check if the path has a given extension.
 * 
 * @param path      Path to check
 * @param ext       Extension to check
 * @return True if path ends in the extension.
 */
static bool has_extension(char *path, char *ext)
{
    char *dot = strrchr(path, '.');
    return ((dot != NULL) && strcmp(dot, ext) == 0);
}

/**
 * @brief Checks and runs shell script shellcmds.
 * 
 * @param t     The shellcmd to handle.
 * @return The exitstatus of the command.
 */
int shellscript_shellcmd(SHELLCMD *t)
{
    if (!file_exists(t->argv[0]))
    {
        return EXIT_FAILURE;
    }

    if (!has_extension(t->argv[0], ".sh"))
    {
        return EXIT_FAILURE;
    }

    SHELLCMD old_t = *t;
    char* name = strdup(argv0);
    char* argv[] = {name, NULL};
    *t = (SHELLCMD){.argc = 1, .argv = argv, .infile = t->argv[0]};
    int exitstatus = execute_shellcmd(t);
    *t = old_t;
    free(name);
    return exitstatus;
}
