/**
 * @file        globals.c
 * @author      Joshua Ng
 * @brief       Global variables.
 * @date        2023-08-18
 */

#include "myshell.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>

/**
 * CITS2002 Project 2 2017
 */

//  THREE INTERNAL VARIABLES (SEE myshell.h FOR EXPLANATION)
char    *HOME, *PATH, *CDPATH;

char    *name0      = NULL;     // the program's name
char    *argv0      = NULL;     // the program's path    
bool    interactive = false;

// ------------------------------------------------------------------------

/**
 * @brief Helper function to ensure that pointer is not null.
 * 
 * @param p     The pointer to check.
 * @param file  The file name of the function call.
 * @param func  The name of the function.
 * @param line  The line number of the function call.
 */
void check_allocation0(void *p, char *file, const char *func, int line)
{
    if (p == NULL) 
    {
        fprintf(stderr, "%s, %s() line %i: unable to allocate memory\n",
            file, func, line);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Helper function to ensure function return is not in error.
 * 
 * @param result    The failure state expression.
 * @param file      The file name of the function call.
 * @param func      The name of the function.
 * @param line      The line number of the function call.
 */
void check_error0(int result, char *file, const char *func, int line)
{
    if (result < 0) 
    {
        fprintf(stderr, "Error %i: %s at %s, %s(): %i\n", 
            errno, strerror(errno), file, func, line);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Helper function to print the command errors.
 * 
 * @param file  The name of the file.
 * @param argv  The argument passed into the file.
 */
void print_command_error(char *file, char *argv)
{
    fprintf(stderr, "%s: %s: %s", file, strerror(errno), argv);
}

/**
 * @brief Prints the redirections infile and/or outfile.
 * 
 * @param t     The shell command to print.
 */
static void print_redirection(SHELLCMD *t)
{
    if (t->infile != NULL)
    {
        printf("< %s ", t->infile);
    }

    if (t->outfile != NULL) 
    {
        if (t->append)
        {
            printf(">>");
        }
        else
        {
            printf(">");
        }

        printf(" %s ", t->outfile);
    }
}

/**
 * @brief Helper function to print the shell comand.
 * 
 * @param t  The shell command to print.
 */
void print_shellcmd0(SHELLCMD *t)
{
    printf("[");

    if(t == NULL) 
    {
        printf("nullcmd ");
        return;
    }

    switch (t->type) 
    {
    case CMD_COMMAND:
    {
        for(int a=0 ; a<t->argc ; a++)
        {
            printf("%s ", t->argv[a]);
        }

        print_redirection(t);
        
        if (t->left != NULL)
        {
            print_shellcmd0(t->left);
        }
        break;
    }
    case CMD_SEMICOLON:
    {
        print_shellcmd0(t->left); 
        printf("; "); 
        print_shellcmd0(t->right);
        break;
    }
    case CMD_AND:
    {
        print_shellcmd0(t->left); 
        printf("&& "); 
        print_shellcmd0(t->right);
        break;
    }
    case CMD_OR:
    {
        print_shellcmd0(t->left); 
        printf("|| "); 
        print_shellcmd0(t->right);
        break;
    }
    case CMD_SUBSHELL:
    {
        printf("( "); 
        print_shellcmd0(t->left); 
        printf(") ");
        print_redirection(t);
        break;
    }
    case CMD_PIPE:
    {
        print_shellcmd0(t->left); 
        printf("| "); 
        print_shellcmd0(t->right);
        break;
    }
    case CMD_BACKGROUND:
    {
        print_shellcmd0(t->left); 
        printf("& "); 
        print_shellcmd0(t->right);
        break;
    }
    default:
    {
        fprintf(stderr, "%s: invalid CMDTYPE in print_shellcmd0()\n", 
            name0);
        exit(EXIT_FAILURE);
        break;
    }
    }

    printf("]");
}
