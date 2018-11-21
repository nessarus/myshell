#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>

//  Written by Chris.McDonald@uwa.edu.au, October 2017
/*
 CITS2002 Project 2 2017
 Name(s):     Minrui Lu (, Joshua Ng)
 Student number(s):   22278526 (, 20163079)
 Date:        3rd-Nov-2017
 */

#if defined(__linux__)
    extern	char	*strdup(const char *str);
    extern	int	fileno(FILE *fp);
    
    extern char *strtok_r(char *, const char *, char **);
    #define BUFFER_SIZE 1024
#endif

// ----------------------------------------------------------------------

#define	DEFAULT_HOME		"/tmp"
#define	DEFAULT_PATH		"/bin:/usr/bin:/usr/local/bin:."
#define	DEFAULT_CDPATH		".:.."
#define COLON               ":"

#define COMMENT_CHAR		'#'	// comment character
#define HOME_CHAR		'~'	// home directory character
#define SLASH_CHAR      '/' // seperator of path

//  ----------------------------------------------------------------------

//  AN enum IN C99 'GENERATES' A SEQUENCE OF UNIQUE, ASCENDING CONSTANTS
typedef enum {
	CMD_COMMAND = 0,	// an actual command node itself
	CMD_SEMICOLON,		// as in   cmd1 ;  cmd2	
	CMD_AND,		// as in   cmd1 && cmd2
	CMD_OR,			// as in   cmd1 || cmd2	
	CMD_SUBSHELL,		// as in   ( cmds )
	CMD_PIPE,		// as in   cmd1 |  cmd2	
	CMD_BACKGROUND		// as in   cmd1 &
} CMDTYPE;


typedef	struct sc {
    CMDTYPE	type;		// the type of the node, &&, ||, etc

    int		argc;		// the number of args iff type == CMD_COMMAND
    char	**argv;		// the NULL terminated argument vector

    char	*infile;	// as in    cmd <  infile
    char	*outfile;	// as in    cmd >  outfile
    bool	append;		// true iff cmd >> outfile

    struct sc	*left, *right;	// pointers to left and right sub-shellcmds
} SHELLCMD;


extern SHELLCMD    *parse_shellcmd(FILE *);    // in parser.c
extern void	free_shellcmd(SHELLCMD *);	// in parser.c
extern int	execute_shellcmd(SHELLCMD *);	// in execute.c
extern char directory[];



/* The global variable HOME points to a directory name stored as a
   character string. This directory name is used to indicate two things:

    - the directory used when the  cd  command is requested without arguments.
    - replacing the leading '~' character in args,  e.g.  ~/filename

   The HOME variable is initialized with the value inherited from the
   invoking environment (or DEFAULT_HOME if undefined).
 */

extern	char	*HOME;

/* The global variables PATH and CDPATH point to character strings representing
   colon separated lists of directory names.

   The value of PATH is used to search for executable files when a command
   name does not contain a '/'

   Similarly, CDPATH provides a colon separated list of directory names
   that are used in an attempt to chage the current working directory.
 */

extern	char	*PATH;
extern	char	*CDPATH;

extern	char	*argv0;		// The name of the shell, typically myshell
extern	bool	interactive;	// true if myshell is connected to a 'terminal'

extern int previous_exitstatus;
extern bool  semicolon_left_execution;


//  ----------------------------------------------------------------------

//  TWO PRE-PROCESSOR MACROS THAT MAY HELP WITH DEBUGGING YOUR CODE.
//      check_allocation(p) ENSURES THAT A POINTER IS NOT NULL, AND
//      print_shellcmd(t)  PRINTS THE REQUESTED COMMAND-TREE
//  THE TWO ACTUAL FUNCTIONS ARE DEFINED IN globals.c

#define	check_allocation(p)	\
	check_allocation0(p, __FILE__, __func__, __LINE__)
extern	void check_allocation0(void *p, char *file, const char *func, int line);

#define	print_shellcmd(t)	\
	printf("called from %s, %s() line %i:\n", __FILE__,__func__,__LINE__); \
	print_shellcmd0(t)
extern	void print_shellcmd0(SHELLCMD *t);
extern int pipe_execution(SHELLCMD *t);
extern char* edit_cd_path(char *cd_path);
extern void cd_not_path_execution(SHELLCMD *t);
extern int time_execution(SHELLCMD *t);
extern void find_path_execute(SHELLCMD *t);
extern void exit_return_value(SHELLCMD *t);
extern void terminate_child(int pid);
extern int sequential_execution(SHELLCMD *t);
extern int  redirection_preparation(SHELLCMD *t);
extern void  execute_command(SHELLCMD *t);
extern void shell_script_execution(SHELLCMD *t);
extern int  background_execution(SHELLCMD *t);
extern void push_pid( pid_t newpid);
extern void pop_pid(pid_t pid);
extern void kill_child(void);

typedef struct _l {
    pid_t       pid;
    struct _l   *next;
} CHILDPIDS;// a struct to store background process pid

extern CHILDPIDS   *cpids;

