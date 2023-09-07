/**
 * @file    redirection.c
 * @author  Joshua Ng
 * @brief   Put the input or output to a given file.
 * @date    2023-09-08
 */

#include "redirection.h"
#include "globals.h"
#include "filepaths.h"
#include "searchpath.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @brief A redirection data structure.
 */
typedef struct REDIRECTION
{
    int old_input;
    int old_output;
} REDIRECTION;

/**
 * @brief Creates the redirection data structure.
 * 
 * @return A memory allocated redirection pointer.
 */
static REDIRECTION* redirection_create(void)
{
    REDIRECTION* redirection = (REDIRECTION*) malloc(sizeof(REDIRECTION));
    check_allocation(redirection);
    *redirection = (REDIRECTION){.old_input  = -1, .old_output = -1};
    return redirection;
}

/**
 * @brief A helper function to open a file and redirect.
 * 
 * @param file      The file to open.
 * @param flags     The file open flags.
 * @param fd_old    The file descriptor to replace.
 * @return A clone of the replaced file descriptor.
 */
int redirection(char* file, int flags, int fd_old)
{
    char *original = file;
    char *temp = NULL;

    if (!is_absolute_path(file))
    {
        temp = searchpath(CDPATH, file);
        if (temp != NULL)
        {
            file = temp;
        }
    }

    int fd = open(file, flags);
    if (fd == -1)
    {
        print_command_error(name0, original);
        return -1;
    }

    int fd_clone = dup(fd_old);
    check_error(fd_clone);
    check_error(dup2(fd, fd_old));
    free(temp);
    return fd_clone;
}

/**
 * @brief Change the input/output node to the requested file descriptor of 
 * provided file.
 * 
 * @param t     The shellcmd to handle.
 * @return Returns the replaced file descriptor struct.
 */
REDIRECTION* redirection_shellcmd(SHELLCMD *t)
{
    REDIRECTION* result = redirection_create();

    if (t->outfile != NULL)
    {
        int append = t->append ? O_APPEND : O_TRUNC;
        int flags = O_CREAT|O_WRONLY|append;

        result->old_output = redirection(t->outfile, flags, STDOUT_FILENO);

        if (result->old_output == -1)
        {
            return NULL;
        }
    }
 
    if (t->infile != NULL)
    {
        result->old_input = redirection(t->infile, O_RDONLY, STDIN_FILENO);
        if (result->old_input == -1)
        {
            free_redirection_shellcmd(t, result);
            return NULL;
        }
    }

    return result;
}

/**
 * @brief Frees and reverts the redirected file descriptors back.
 * 
 * @param t     The shellcmd to handle.
 * @param r     The redirected file descriptors.
 */
void free_redirection_shellcmd(SHELLCMD *t, REDIRECTION* r)
{
    if (r->old_input != -1)
    {
        check_error(dup2(r->old_input, STDIN_FILENO));
    }

    if (r->old_output != -1)
    {
        check_error(dup2(r->old_output, STDOUT_FILENO));
    }

    free(r);
}

