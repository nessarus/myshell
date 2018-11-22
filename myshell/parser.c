#include "myshell.h"

//  Written by Chris.McDonald@uwa.edu.au, October 2017

/*  This file provides the most complicated part of the shell -
    its command parser.  This file provides two functions (at bottom of file):

	    SHELLCMD	*parse_shellcmd(FILE *fp);
    and	    void	free_shellcmd(SHELLCMD *t);

    These two functions need only be called from within the main() function.
    All other functions are variables are declared as 'static' so that they
    are not visible outside of this file.

    REMEMBER, THERE IS NO NEED FOR YOU TO UNDERSTAND OR MODIFY THIS CODE.
 */
/*
 CITS2002 Project 2 2017
 Name(s):     Minrui Lu (, Joshua Ng)
 Student number(s):   22278526 (, 20163079)
 Date:        3rd-Nov-2017
 */
#include <signal.h>
#include <setjmp.h>

// ----------------------- parsing definitions --------------------------

typedef enum {
	T_AND = 0,
	T_APPEND,
	T_BACKGROUND,
	T_BAD,
	T_DQUOTE,
	T_EOF,
	T_FROMFILE,
	T_LEFTB,
	T_NL,
	T_OR,
	T_PIPE,
	T_RIGHTB,
	T_SCOLON,
	T_SQUOTE,
	T_TOFILE,
	T_WORD,
} TOKEN;

#define	is_redirection(t) (t == T_FROMFILE || t == T_TOFILE || t == T_APPEND)
#define	is_word(t)        (t == T_WORD     || t == T_DQUOTE || t == T_SQUOTE)

// -------------------------- lexical stuff -----------------------------

static	FILE	*fp;

static	TOKEN	token;
static	char	chararray[BUFSIZ];
static	char	line[BUFSIZ];
static	char	*cp;

static	int	nextch;
static	int	cc;
static	int	ll;
static	int	lc;

static	int	prompt_no	= 1;
static	int	nerrors		= 0;

static void get(void)			// get next buffered char from line
{
    if(cc == ll) {
	if(interactive) {
	    static char	PS1[32], PS2[32];

	    if(lc == 1) {
		sprintf(PS1, "\n%s.%i ", argv0, prompt_no);	// format prompt

		char *s = PS1;
		while(*s)
		    s++;

		strcpy(PS2, " ++          ");
		PS2[ s-PS1-1 ] = '\0';
	    }
	    fputs(lc == 1 ? PS1 : PS2 , stdout);
	}
	fgets(line, sizeof line, fp);
	ll	= strlen(line);
	cc	= 0;
	lc++;
    }
    nextch = line[cc++];
}

#define unget()		(nextch = line[--cc])

static void skip_blanks(void)		//  Skip spaces, tabs and comments
{
    while(nextch == ' ' || nextch == '\t' || nextch == COMMENT_CHAR) {
	if(nextch == COMMENT_CHAR)	//  ignore to end-of-line
	    while (nextch != '\n') 
		get();
	get();
    }
}

static void escape_char(void)
{
    get();
    switch(nextch) {
	case 'b' :  *cp++ = '\b'; break;
	case 'f' :  *cp++ = '\f'; break;
	case 'n' :  *cp++ = '\n'; break;
	case 'r' :  *cp++ = '\r'; break;
	case 't' :  *cp++ = '\t'; break;
	default  :  *cp++ = nextch;
    }
    get();
}

static void gettoken(void)
{
    *chararray	= '\0';
    get();
    skip_blanks();
    if(feof(fp)) {
	token = T_EOF;
	return;
    }
    else if(nextch == '<') {		// input redirection 
	token = T_FROMFILE;
	return;
    }
    else if(nextch == '>') {		// output redirection 
	get();
	if(nextch == '>') {
	    token = T_APPEND;
	}
	else {
	    unget();
	    token = T_TOFILE;
	}
	return;
    }
    else if(nextch == ';') {		// sequential
	token = T_SCOLON;
	return;
    }
    else if(nextch == '&') {		// and-conditional
	get();
	if(nextch == '&')
	    token = T_AND;
	else {
	    unget();			// background
	    token = T_BACKGROUND;
	}
    }
    else if(nextch == '|') {		// or-conditional
	get();
	if(nextch == '|') {
	    token = T_OR;
	}
	else {				// pipe operator
	    unget();
	    token = T_PIPE;
	}
	return;
    }
    else if(nextch == '(') {		// subshells
	token = T_LEFTB;
	return;
    }
    else if(nextch == ')') {		// subshells
	token = T_RIGHTB;
	return;
    }
    else if(nextch == '\n') {		// newline => sequential
	token = T_NL;
	return;
    }
    else if(nextch == '"' || nextch == '\'') {
	*chararray = nextch;
	cp = chararray+1;
	do {
	    get();
	    while(nextch == '\\')
		escape_char();
	    *cp++ = nextch;
	}   while(nextch != *chararray && !feof(fp));
	*--cp = '\0';
	token = *chararray == '"' ? T_DQUOTE : T_SQUOTE;
	return;
    }
    else {
	cp = chararray;
	while(!feof(fp)  && !strchr(" \t\n<>|();&",nextch)) {
	    while(nextch == '\\')
		escape_char();
	    *cp++ = nextch;
	    get();
	}
	unget();
	*cp = '\0';
	token = T_WORD;
	return;
    }
}

// -------------------- parsing code (at last!) ------------------------ 

static	jmp_buf	env;

static void interrupt_parsing(int why)
{
    if(why == SIGINT && interactive)
	fputc('\n', stdout);
    longjmp(env, 1);
}

static SHELLCMD *new_shellcmd(CMDTYPE t)
{
    SHELLCMD	*t1 = calloc(1, sizeof(*t1));

    check_allocation(t1);
    t1->type	= t;
    return t1;
}

static bool get_redirection(SHELLCMD *t1)
{
    char	*filename;
    TOKEN	cptoken	= token;

    gettoken();
    if(token == T_WORD) {
	filename = strdup(chararray);
	check_allocation(filename);
    }
    else if(token == T_SQUOTE || token == T_DQUOTE) {
	filename = strdup(chararray+1);
	check_allocation(filename);
    }
    else {
	fprintf(stderr, "%s redirection filename expected\n",
				cptoken == T_FROMFILE ? "input" : "output");
	nerrors++;
	return false;
    }

    if(cptoken == T_FROMFILE) {
	if(t1->infile != NULL) {
	    fprintf(stderr, "multiple input redirection\n");
	    nerrors++;
	    return false;
	}
	t1->infile = filename;
    }
    else {
	if(t1->outfile != NULL) {
	    fprintf(stderr, "multiple output redirection\n");
	    nerrors++;
	    return false;
	}
	t1->append	= (cptoken == T_APPEND);
	t1->outfile	= filename;
    }
    return true;
}

static SHELLCMD *cmd_wordlist(void)
{
#define	MAXARGS		254		// hope that this will be enough!

    static char	*argv[MAXARGS+2];

    int		argc	= 0;
    SHELLCMD	*t1	= new_shellcmd(CMD_COMMAND);

    while(!feof(fp) && (is_redirection(token) || is_word(token))) {
	switch ((int)token) {
	case T_WORD :
	    if(argc < MAXARGS) {
		if(chararray[0] == HOME_CHAR) {
		    argv[argc]	= malloc(strlen(HOME) + strlen(chararray) + 1);
		    check_allocation(argv[argc]);
		    sprintf(argv[argc], "%s%s", HOME, chararray+1);
		}
		else {
		    argv[argc]	= strdup(chararray);
		    check_allocation(argv[argc]);
		}
		++argc;
		argv[argc] = NULL;
	    }
	    break;
	case T_SQUOTE :
	case T_DQUOTE :
	    if(argc < MAXARGS) {
		argv[argc] = strdup(chararray+1);
		check_allocation(argv[argc]);
		++argc;
		argv[argc] = NULL;
	    }
	    break;
	case T_FROMFILE :
	case T_TOFILE :
	case T_APPEND :
	    if(get_redirection(t1) == false) {
		free_shellcmd(t1);
		interrupt_parsing(0);
	    }
	    break;
	}
	gettoken();
    }
    if(argc == 0) {
	free(t1);
	return NULL;
    }
    argv[argc] = NULL;
    t1->argc	= argc;
    t1->argv	= malloc(((argc+1) * sizeof(t1->argv[0])));
    check_allocation(t1->argv);

    while(argc >= 0) {
	t1->argv[argc] = argv[argc];
	--argc;
    }
    return t1;
#undef	MAXARGS
}

static SHELLCMD *cmd_pipeline(void);		// a forward declaration

static SHELLCMD *cmd_condition(void)
{
    SHELLCMD		*t1, *t2;

    t1 = cmd_pipeline();
    while(token == T_AND || token == T_OR) {
	TOKEN savetoken	= token;

        t2		= new_shellcmd(token == T_AND ? CMD_AND : CMD_OR);
	t2->left	= t1;
	gettoken();
	t2->right	= cmd_pipeline();
	if(t2->right == NULL) {
	    fprintf(stderr, "command expected after '%s'\n",
				(savetoken == T_AND) ? "&&" : "||");
	    nerrors++;
	    free_shellcmd(t2);
	    interrupt_parsing(0);
	}
	t1		= t2;
    }
    return t1;
}

static SHELLCMD *cmd_sequence(void)
{
    SHELLCMD		*t1, *t2;

    t1 = cmd_condition();
    while(token == T_SCOLON || token == T_BACKGROUND) {
        t2  = new_shellcmd(token == T_SCOLON ? CMD_SEMICOLON : CMD_BACKGROUND);
	t2->left	= t1;
	gettoken();
	t2->right	= cmd_condition();
	t1		= t2;
    }
    return t1;
}

static SHELLCMD *cmd_factor(void)
{
    SHELLCMD		*t1, *t2;

    if(token == T_LEFTB) {
	gettoken();
	t1 = cmd_sequence();
	if(token != T_RIGHTB) {
	    fprintf(stderr, "')' expected\n");
	    nerrors++;
	    free_shellcmd(t1);
	    interrupt_parsing(0);
	}
	else {
	    if(t1 == NULL) {
		fprintf(stderr, "subshells may not be empty\n");
		nerrors++;
		interrupt_parsing(0);
	    }
	    t2		= new_shellcmd(CMD_SUBSHELL);
	    t2->left	= t1;
	    t2->right	= NULL;
	    gettoken();
	    while(is_redirection(token)) {
		if(get_redirection(t2) == false) {
		    free_shellcmd(t2);
		    interrupt_parsing(0);
		}
		gettoken();
	    }
	    t1		= t2;
	}
    }
    else
	t1 = cmd_wordlist();
    return t1;
}

static SHELLCMD *cmd_pipeline(void)
{
    SHELLCMD		*t1, *t2;

    t1 = cmd_factor();
    if(token == T_PIPE) {

	if(t1 != NULL && t1->outfile != NULL) {
	   fprintf(stderr, "output cannot be both redirected and piped\n");
	    nerrors++;
	    free_shellcmd(t1);
	    interrupt_parsing(0);
	}
        t2		= new_shellcmd(CMD_PIPE);
	t2->left	= t1;
	gettoken();
	t2->right	= cmd_pipeline();

	if(t2->right == NULL) {
	    fprintf(stderr, "command expected after '|'\n");
	    nerrors++;
	    free_shellcmd(t2);
	    interrupt_parsing(0);
	}

	if(t2->right != NULL && t2->right->infile != NULL) {
	    fprintf(stderr, "input cannot be both redirected and piped\n");
	    nerrors++;
	    free_shellcmd(t2);
	    interrupt_parsing(0);
	}
	t1		= t2;
    }
    return t1;
}

// -------------------------- parsing stuff -----------------------------

typedef	void	(*sighandler_t)(int);


#if	defined(CGI_INTERFACE)
SHELLCMD *parse_shellcmd_string(char *str)
{
    extern FILE		*fp;			// in parser.c
    extern SHELLCMD	*cmd_sequence(void);	// in parser.c

    SHELLCMD	*t1;

    sighandler_t	old_handler;

    old_handler		= signal(SIGINT, interrupt_parsing);
    if(setjmp(env)) {
	return NULL;
    }

    fp		= stdin;

    sprintf(line, "%s\n", str);
    ll		= strlen(line);
    cc		= 0;
    lc		= 1;
    nerrors	= 0;
    gettoken();
    t1		= cmd_sequence();

    if(token != T_NL && token != T_EOF) {
	fputs("garbage at end of line\n", stderr);
	nerrors++;
    }
    return (nerrors == 0 ? t1 : NULL);
}

#else

//  READ INPUT FROM THE INDICATED FILE-POINTER AND RETURN A COMMAND-TREE
//  ANTICIPATED THAT THIS FUNCTION WILL BE CALLED FROM main()
SHELLCMD *parse_shellcmd(FILE *_fp)
{
    SHELLCMD		*t1;
    sighandler_t	old_handler;

    old_handler		= signal(SIGINT, interrupt_parsing);
    setjmp(env);
    do {
	t1		= NULL;
	fp		= _fp;
	cc		= 0;
	ll		= 0;
	lc		= 1;
	nerrors		= 0;
	if(feof(fp)) {
	    break;
	}
	gettoken();
    } while((t1 = cmd_sequence()) == NULL);
    signal(SIGINT, old_handler);	// control-C to interrupt parsing

    ++prompt_no;

    if(token != T_NL && token != T_EOF) {
	fprintf(stderr, "garbage at end of line\n");
	nerrors++;
    }
    return (nerrors == 0 ? t1 : NULL);
}
#endif

//  DEALLOCATE ALL DYNAMICALLY ALLOCATED MEMORY IN A COMMAND-TREE
//  ANTICIPATED THAT THIS FUNCTION WILL BE CALLED FROM main()
static void free_redirection(SHELLCMD *t)
{
    if(t->infile)
	free(t->infile);
    if(t->outfile)
	free(t->outfile);
}

void free_shellcmd(SHELLCMD *t)
{
    if(t != NULL) {
	switch (t->type) {
	case CMD_COMMAND :
	    if(t->argc > 0) {			// free command-words
		for(int a=0 ; a<t->argc ; a++)
		    free(t->argv[a]);
		free(t->argv);
	    }
	    free_redirection(t);
	    break;
	case CMD_SUBSHELL :
	    free_shellcmd(t->left);
	    free_redirection(t);
	    break;
	case CMD_AND :
	case CMD_BACKGROUND :
	case CMD_OR :
	case CMD_PIPE :
	case CMD_SEMICOLON :
	    free_shellcmd(t->left);		// free the subtrees
	    free_shellcmd(t->right);
	    break;
	}
	free(t);				// and free the node itself
    }
}
