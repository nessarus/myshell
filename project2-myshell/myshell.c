#include "myshell.h"

/*
 CITS2002 Project 2 2017
 Name(s):     Minrui Lu (, Joshua Ng)
 Student number(s):   22278526 (, 20163079)
 Date:        3rd-Nov-2017
 */

int check_exit(int exitstatus, int previous_exitstatus)
{
    if(exitstatus == 3){
        exit(previous_exitstatus);
    }
    return exitstatus;
}


int main(int argc, char *argv[])
{
    getcwd(directory,MAXPATHLEN);// get the current working directory
    strcat(directory,"/myshell");
    //  REMEMBER THE PROGRAM'S NAME (TO REPORT ANY LATER ERROR MESSAGES)
    argv0	= (argv0 = strrchr(argv[0],'/')) ? argv0+1 : argv[0];
    argc--;				// skip 1st command-line argument
    argv++;

//  INITIALIZE THE THREE INTERNAL VARIABLES
    HOME	= getenv("HOME");
    if(HOME == NULL) {
	HOME	= DEFAULT_HOME;
    }

    PATH	= getenv("PATH");
    if(PATH == NULL) {
	PATH	= DEFAULT_PATH;
    }

    CDPATH	= getenv("CDPATH");
    if(CDPATH == NULL) {
        CDPATH = DEFAULT_PATH;
    }

//  DETERMINE IF THIS SHELL IS INTERACTIVE
    interactive		= (isatty(fileno(stdin)) && isatty(fileno(stdout)));

    int exitstatus	= EXIT_SUCCESS;

//  READ AND EXECUTE COMMANDS FROM stdin UNTIL IT IS CLOSED (with control-D)
    while(!feof(stdin)) {
        SHELLCMD	*t = parse_shellcmd(stdin);
        if(t != NULL) {

//  WE COULD DISPLAY THE PARSED COMMAND-TREE, HERE, BY CALLING:
//	    print_shellcmd(t);

            previous_exitstatus = exitstatus;
            exitstatus = execute_shellcmd(t);

            free_shellcmd(t);
        }
    }
    if(interactive) {
	fputc('\n', stdout);
    }
    
    return exitstatus;
}
