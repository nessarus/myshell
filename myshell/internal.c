/**
 * @file    internal.c
 * @author  Joshua Ng
 * @brief   Execute internal commands.
 * @date    2023-08-17
 */

#include "internal.h"
#include "myshell.h"
#include "globals.h"
#include "filepaths.h"
#include "searchpath.h"
#include "simplemap.h"
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

/**
 * @brief Parser for internal command enums.
 */
typedef struct SIMPLEMAP* CMDPARSER;

/**
 * @brief Get the cmd parser object.
 * 
 * @return A static cmd parser object.
 */
static CMDPARSER get_cmd_parser(void) 
{
    static CMDPARSER map;

    if (map == NULL)
    {
        map = simplemap_create();
        simplemap_insert(map, "cd", (int) COMMAND_CD);
        simplemap_insert(map, "exit", (int) COMMAND_EXIT);
        simplemap_insert(map, "time", (int) COMMAND_TIME);
    }

    return map;
}

/**
 * @brief Looks up a string command and returns command enum.
 * 
 * @param command   The string command to lookup.
 * @return The requested command enum.
 */
COMMAND parse_cmd(char* command)
{
    CMDPARSER cmd_parser = get_cmd_parser();
    int index = simplemap_search(cmd_parser, command);

    if (index != -1)
    {
        return (COMMAND) simplemap_getvalue(cmd_parser, index);
    }
    
    return COMMAND_EXECUTE;
}

/**
 * @brief Handles the exit command.
 * 
 * @param t     The exit shellcmd to handle.
 * @return The exitstatus of the operation.
 */
int exit_shellcmd(SHELLCMD *t, int exitstatus)
{
    if (t->argc > 1)
    {
        if (strcmp(t->argv[1], "0") == 0)
        {
            exitstatus = EXIT_SUCCESS;
        }
        else if(strcmp(t->argv[1], "1") == 0)
        {
            exitstatus = EXIT_FAILURE;
        }
    }

    exit(exitstatus);
}

/**
 * @brief Handles the change directory command.
 * 
 * @param t     The change directory shellcmd.
 * @return The exitstatus of the operation. 
 */
int cd_shellcmd(SHELLCMD *t)
{
    char *filepath = t->argv[1];
    char *original = (t->argv[1] == NULL) ? "" : t->argv[1];
    char *temp = NULL;

    if (t->argc < 2)
    {
        filepath = HOME;
    }
    else if (!is_absolute_path(t->argv[1]))
    {
        temp = searchpath(CDPATH, t->argv[1]);
        if (temp != NULL)
        {
            filepath = temp;
        }
    }

    int result = chdir(filepath);
    free(temp);

    if (result == -1)
    {
        print_command_error(t->argv[0], original);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Handles the time command.
 * 
 * @param t     The time shellcmd to handle.
 * @return The exitstatus of the operation. 
 */
int time_shellcmd(SHELLCMD *t)
{
    if ((t->argc == 1) && (t->left == NULL))
    {
        fprintf(stderr, "\n 0 msec\n");
        return EXIT_SUCCESS;
    }

    struct timeval start;
    struct timeval end;
    long elapsed = 0;
    int exitstatus = EXIT_SUCCESS;

    check_error(gettimeofday(&start, NULL));

    if (t->argc > 1)
    {
        t->argc--;
        t->argv++;
        exitstatus = execute_shellcmd(t);
        t->argc++;
        t->argv--;
    }
    else if (t->left != NULL)
    {
        exitstatus = execute_shellcmd(t->left);
    }

    check_error(gettimeofday(&end, NULL));

    // Take difference in time in milliseconds.
    elapsed = (end.tv_sec - start.tv_sec) * 1000000;
    elapsed += end.tv_usec - start.tv_usec;
    elapsed /= 1000;

    check_error(fprintf(stderr, "\n %ld msec\n", elapsed));
    return exitstatus;
}
