#pragma once
/**
 * @file    myshell.h
 * @author  Joshua Ng
 * @brief   provides the definition of myshell's user-defined datatypes 
 *              and the declaration of global variables.
 * @date    2023-08-18
 */

#include <stdbool.h>
#include <stdio.h>

//  Written by Chris.McDonald@uwa.edu.au, October 2017

// ----------------------------------------------------------------------

#define DEFAULT_HOME    "/tmp"
#define DEFAULT_PATH    "/bin:/usr/bin:/usr/local/bin:."
#define DEFAULT_CDPATH  ".:.."
#define COLON           ":"

#define COMMENT_CHAR    '#'    // comment character
#define HOME_CHAR       '~'    // home directory character

//  ----------------------------------------------------------------------

//  AN enum IN C99 'GENERATES' A SEQUENCE OF UNIQUE, ASCENDING CONSTANTS
typedef enum 
{
    CMD_COMMAND = 0,  // an actual command node itself
    CMD_SEMICOLON,    // as in   cmd1 ;  cmd2    
    CMD_AND,          // as in   cmd1 && cmd2
    CMD_OR,           // as in   cmd1 || cmd2    
    CMD_SUBSHELL,     // as in   ( cmds )
    CMD_PIPE,         // as in   cmd1 |  cmd2    
    CMD_BACKGROUND    // as in   cmd1 &
} CMDTYPE;

typedef struct sc 
{
    CMDTYPE type;       // the type of the node, &&, ||, etc

    int     argc;       // the number of args iff type == CMD_COMMAND
    char    **argv;     // the NULL terminated argument vector

    char    *infile;    // as in    cmd <  infile
    char    *outfile;   // as in    cmd >  outfile
    bool    append;     // true iff cmd >> outfile

    struct sc *left, *right;    // pointers to left and right sub-shellcmds
} SHELLCMD;

int execute_shellcmd(SHELLCMD *);

/**
 * @brief The global variable HOME points to a directory name stored as a
 * character string. This directory name is used to indicate two things:
 *  - the directory used when the  cd  command is requested without arguments.
 *  - replacing the leading '~' character in args,  e.g.  ~/filename
 * The HOME variable is initialized with the value inherited from the
 * invoking environment (or DEFAULT_HOME if undefined).
 */
extern char *HOME;

/**
 * The global variables PATH and CDPATH point to character strings 
 * representing colon separated lists of directory names.
 * 
 * The value of PATH is used to search for executable files when a command
 * name does not contain a '/'.
 * 
 * Similarly, CDPATH provides a colon separated list of directory names
 * that are used in an attempt to chage the current working directory.
 */
extern char *PATH;
extern char *CDPATH;

extern char *name0;         // The name of the shell, typically myshell
extern char *argv0;         // The path of the shell
extern bool interactive;    // True if myshell is connected to a 'terminal'

