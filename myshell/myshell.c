/**
 * @file    myshell.c
 * @author  Joshua Ng
 * @brief   myshell is a program that supports a small subset of the 
 *          features of a standard Unix-based shell.
 * @date    2023-08-23
 */

#if defined(__linux__)
    extern int fileno(FILE *fp);
#endif

#include "myshell.h"
#include "globals.h"
#include "parser.h"
#include "external.h"
#include "internal.h"
#include "subshell.h"
#include "redirection.h"
#include "pipeline.h"
#include "background.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief The exit status to return on exit.
 */
static int exitstatus = EXIT_SUCCESS;

/**
 * @brief This function should traverse the command-tree and execute the
 * commands that it holds, returning the appropriate exit-status.
 * 
 * @param t     The shellcmd to handle.
 * @return The exitstatus of the operation.
 */
int execute_shellcmd(SHELLCMD *t)
{
    switch (t->type)
    {
    case CMD_COMMAND:
    {
        struct REDIRECTION* redirection = redirection_shellcmd(t);

        if (redirection == NULL)
        {
            return EXIT_FAILURE;
        }

        COMMAND command = parse_cmd(t->argv[0]);

        switch (command)
        {
        case COMMAND_CD:
            exitstatus = cd_shellcmd(t);
            break;
        case COMMAND_EXIT:
            background_exit();
            exitstatus = exit_shellcmd(t, exitstatus);
            break;
        case COMMAND_TIME:
            exitstatus = time_shellcmd(t);
            break;
        case COMMAND_EXECUTE:
        default:
            exitstatus = external_shellcmd(t);
            break;
        }

        free_redirection_shellcmd(t, redirection);
        break;
    }
    case CMD_SEMICOLON:    // cmd1 ;  cmd2
        exitstatus = execute_shellcmd(t->left);
        exitstatus = execute_shellcmd(t->right);
        break;
    case CMD_AND:          // cmd1 && cmd2
        exitstatus = execute_shellcmd(t->left);
        if (exitstatus == EXIT_SUCCESS)
        {
            execute_shellcmd(t->right);
        }
        break;
    case CMD_OR:           // cmd1 || cmd2
        exitstatus = execute_shellcmd(t->left);
        if (exitstatus != EXIT_SUCCESS)
        {
            execute_shellcmd(t->right);
        }
        break;
    case CMD_SUBSHELL:     // ( cmds )
    {
        struct REDIRECTION* redirection = redirection_shellcmd(t);

        if (redirection == NULL)
        {
            return EXIT_FAILURE;
        }

        exitstatus = subshell_shellcmd(t);
        free_redirection_shellcmd(t, redirection);
        break;
    }
    case CMD_PIPE:         // cmd1 |  cmd2    
        exitstatus = pipeline_shellcmd(t);
        break;
    case CMD_BACKGROUND:   // cmd1 &
    {
        exitstatus = background_shellcmd(t->left);
        exitstatus = execute_shellcmd(t->right);
        break;
    }
    default:
    {
        break;
    }
    }
    return exitstatus;
}

/**
 * myshell is a program that supports a small subset of the features of a 
 * standard Unix-based shell.
 * 
 * CITS2002 Project 2 2017
 * Name(s):             Joshua Chin Hao Ng
 * Student number(s):   20163079
 * Date:                06/08/23
 */
int main(int argc, char *argv[])
{
    // REMEMBER THE PROGRAM'S NAME (TO REPORT ANY LATER ERROR MESSAGES)
    name0 = (name0 = strrchr(argv[0], '/')) ? (name0 + 1) : argv[0];
    argv0 = argv[0];
    argc--;             // skip 1st command-line argument
    argv++;

    // INITIALIZE THE THREE INTERNAL VARIABLES
    HOME = getenv("HOME");
    if (HOME == NULL)
    {
        HOME = DEFAULT_HOME;
    }

    PATH = getenv("PATH");
    if (PATH == NULL) 
    {
        PATH = DEFAULT_PATH;
    }

    CDPATH = getenv("CDPATH");
    if (CDPATH == NULL) 
    {
        CDPATH = DEFAULT_CDPATH;
    }

    // DETERMINE IF THIS SHELL IS INTERACTIVE
    interactive = (isatty(fileno(stdin)) && isatty(fileno(stdout)));

    // READ AND EXECUTE COMMANDS FROM stdin UNTIL IT IS CLOSED (with control-D)
    while (!feof(stdin))
    {
        SHELLCMD *t = parse_shellcmd(stdin);

        if (t == NULL)
        {
            continue;
        }

        // WE COULD DISPLAY THE PARSED COMMAND-TREE, HERE, BY CALLING:
        // print_shellcmd(t);

        exitstatus = execute_shellcmd(t);
        free_shellcmd(t);
    }

    if (interactive) 
    {
	    fputc('\n', stdout);
    }

    return exitstatus;
}
