
/**
 * @file        parser.c
 * @author      Joshua Ng
 * @brief       Modified to fix errors.
 * @date        2023-08-09
 */

#include "parser.h"
#include "globals.h"
#include "myshell.h"
#include <stdlib.h>
#include <string.h>

/**
 * Written by Chris.McDonald@uwa.edu.au, October 2017.
 * 
 * This file provides the most complicated part of the shell -
 * its command parser.  This file provides two functions (at bottom of file):
 *      
 *      SHELLCMD    *parse_shellcmd(FILE *fp);
 *      void        free_shellcmd(SHELLCMD *t);
 * 
 * These two functions need only be called from within the main() function.
 * All other functions are variables are declared as 'static' so that they
 * are not visible outside of this file.
 */

#include <signal.h>
#include <setjmp.h>

// ----------------------- parsing definitions --------------------------

typedef enum 
{
    T_AND = 0,
    T_APPEND,
    T_BACKGROUND,
    T_BAD,
    T_DQUOTE,
    T_EOF,
    T_FROMFILE,
    T_LEFTB,
    T_NEWLINE,
    T_OR,
    T_PIPE,
    T_RIGHTB,
    T_SCOLON,
    T_SQUOTE,
    T_TOFILE,
    T_WORD,
} TOKEN;

#define    is_redirection(t) (t == T_FROMFILE || t == T_TOFILE || t == T_APPEND)
#define    is_word(t)        (t == T_WORD || t == T_DQUOTE || t == T_SQUOTE)

// -------------------------- lexical stuff -----------------------------

static  FILE    *fp;

static  TOKEN   token;
static  char    chararray[BUFSIZ];
static  char    *ch_ptr;

static  char    line[BUFSIZ];
static  char    prompt1[32], prompt2[32];
static  char    ch;
static  size_t  ch_count;
static  size_t  line_length;
static  bool    init_prompt;

static  uint32_t prompt_no   = 1;
static  uint32_t nerrors = 0;

/**
 * @brief Get the next buffered char from line
 */
static void get(void)
{
    if (ch_count >= line_length)
    {
        ch = '\0';
        line[0] = '\0';
        line_length = 0;
        ch_count = 0;
        
        if (interactive && init_prompt)
        {
            // format prompt
            sprintf(prompt1, "\n%s.%i ", name0, prompt_no);
            strcpy(prompt2, " ++          ");
            prompt2[strlen(prompt1) - 1] = '\0';
            fputs(prompt1, stdout);
        }
        
        if (fgets(line, sizeof(line), fp) == NULL)
        {
            init_prompt = false;
            return;
        }
        
        if (interactive && !init_prompt)
        {
            fputs(prompt2 , stdout);
        }
        init_prompt = false;
        
        line_length = (size_t) strlen(line);
    }
    
    ch = line[ch_count++];
}

/**
 * @brief Rewinds get's next buffered char to the previous one
 */
#define unget() (ch = line[--ch_count])

/**
 * @brief Skip spaces, tabs and comments
 */
static void skip_blanks(void)
{
    while (ch == ' ' || ch == '\t' || ch == COMMENT_CHAR)
    {
        // ignore to end-of-line
        if (ch == COMMENT_CHAR)
        {
            while (ch_count < line_length)
            {
                get();
            }
        }

        get();
    }
}

/**
 * @brief Handles escape characters.
 */
static void escape_char(void)
{
    get();

    switch (ch)
    {
        case 'b': *ch_ptr++ = '\b'; break;
        case 'f': *ch_ptr++ = '\f'; break;
        case 'n': *ch_ptr++ = '\n'; break;
        case 'r': *ch_ptr++ = '\r'; break;
        case 't': *ch_ptr++ = '\t'; break;
        default : *ch_ptr++ = ch;
    }

    get();
}

/**
 * @brief parse the line for the token type.
 */
static void gettoken(void)
{
    *chararray = '\0';
    get();
    skip_blanks();

    if (feof(fp)) 
    {
        token = T_EOF;
        return;
    }
    
    switch (ch)
    {
    case '<':   // input redirection 
        token = T_FROMFILE;
        break;
    case '>':   // output redirection 
        token = T_APPEND;
        get();
        if (ch != '>') 
        {
            unget();
            token = T_TOFILE;
        }
        break;
    case ';':   // sequential
        token = T_SCOLON;
        break;
    case '&':   // and-conditional
        token = T_AND;
        get();
        if(ch != '&')
        {
            unget(); // background
            token = T_BACKGROUND;
        }
        break;
    case '|':   // or-conditional
        token = T_OR;
        get();
        
        if (ch != '|') // pipe operator
        {  
            unget();
            token = T_PIPE;
        }
        break;
    case '(':   // subshells
        token = T_LEFTB;
        break;
    case ')':   // subshells
        token = T_RIGHTB;
        break;
    case '\n':
        token = T_NEWLINE;
        break;
    case '"':
    case '\'':
        *chararray = ch;
        ch_ptr = chararray + 1;

        do 
        {
            get();
            while (ch == '\\')
            {
                escape_char();
            }
            *ch_ptr++ = ch;
        } 
        while((ch != *chararray) && !feof(fp));

        *--ch_ptr = '\0';
        token = (*chararray == '"') ? T_DQUOTE : T_SQUOTE;
        break;
    default:
        ch_ptr = chararray;

        while (!feof(fp)  && !strchr(" \t\n<>|();&", ch)) 
        {
            while (ch == '\\')
            {
                escape_char();
            }
            *ch_ptr++ = ch;
            get();
        }

        unget();
        *ch_ptr = '\0';
        token = T_WORD;
    }
}

// -------------------- parsing code (at last!) ------------------------ 

static jmp_buf env;

/**
 * @brief Handler to interrupt parsing error.
 * 
 * @param why   The interrupt signal.
 */
static void interrupt_parsing(int why)
 {
    if ((why == SIGINT) && interactive)
    {
 	    fputc('\n', stdout);
    }

    longjmp(env, 1);
 }

/**
 * @brief Create a new shellcmd struct.
 * 
 * @param t The command type.
 * @return A memory allocated pointer to a shellcmd stuct.
 */
static SHELLCMD *new_shellcmd(CMDTYPE t)
{
    SHELLCMD *t1 = calloc(1, sizeof(*t1));
    check_allocation(t1);
    t1->type = t;
    return t1;
}

/**
 * @brief Get the input and output filenames on redirection.
 * 
 * @param t1    The shellcmd to update.
 * @return True if the redirection parse has no errors.     
 */
static bool get_redirection(SHELLCMD *t1)
{
    char *filename;
    TOKEN cptoken = token;

    gettoken();
    if (token == T_WORD) 
    {
        filename = strdup(chararray);
        check_allocation(filename);
    }
    else if (token == T_SQUOTE || token == T_DQUOTE) 
    {
        filename = strdup(chararray + 1);
        check_allocation(filename);
    }
    else 
    {        
        fprintf(stderr, "%s redirection filename expected\n",
            (cptoken == T_FROMFILE) ? "input" : "output");
        nerrors++;
        return false;
    }

    if (cptoken == T_FROMFILE) 
    {
        if (t1->infile != NULL) 
        {
            fprintf(stderr, "multiple input redirection\n");
 	        nerrors++;
            return false;
        }

        t1->infile = filename;
    }
    else
    {
        if(t1->outfile != NULL) 
        {
            fprintf(stderr, "multiple output redirection\n");
 	        nerrors++;
            return false;
        }

        t1->append = (cptoken == T_APPEND);
        t1->outfile = filename;
    }

    return true;
}

/**
 * @brief Creates a word list for each shellcmd.
 * 
 * @return A memory allocated pointer to a shellcmd struct.
 */
static SHELLCMD *cmd_wordlist(void)
{
#define MAXARGS 254     // hope that this will be enough!

    static char *argv[MAXARGS + 2];
    int argc = 0;
    SHELLCMD *t1 = new_shellcmd(CMD_COMMAND);

    while (!feof(fp) && (is_redirection(token) || is_word(token))) 
    {
        switch ((int) token) 
        {
        case T_WORD :
            if (argc < MAXARGS) 
            {
                if (chararray[0] == HOME_CHAR) 
                {
                    argv[argc] = malloc(strlen(HOME) + strlen(chararray) + 1);
                    check_allocation(argv[argc]);
                    sprintf(argv[argc], "%s%s", HOME, chararray + 1);
                }
                else 
                {
                    argv[argc] = strdup(chararray);
                    check_allocation(argv[argc]);
                }

                ++argc;
                argv[argc] = NULL;
            }
            break;
        case T_SQUOTE :
        case T_DQUOTE :
            if (argc < MAXARGS) 
            {
                argv[argc] = strdup(chararray + 1);
                check_allocation(argv[argc]);
                ++argc;
                argv[argc] = NULL;
            }
            break;
        case T_FROMFILE :
        case T_TOFILE :
        case T_APPEND :
            if (!get_redirection(t1))
            {
                free_shellcmd(t1);
                interrupt_parsing(0);
            }
            break;
        default:
            break;
        }

        gettoken();
    }

    if(argc == 0) 
    {
        free_shellcmd(t1);
        return NULL;
    }

    argv[argc] = NULL;
    t1->argc = argc;
    t1->argv = malloc((argc + 1) * sizeof(t1->argv[0]));
    check_allocation(t1->argv);

    for (int i = 0; i < (argc + 1); i++)
    {
        t1->argv[i] = argv[i];
    }

    return t1;
#undef    MAXARGS
}

static SHELLCMD *cmd_pipeline(void);        // a forward declaration

/**
 * @brief Constructs conditional shellcmds. i.e. &&, ||
 * 
 * @return A memory allocated pointer to a shellcmd.
 */
static SHELLCMD *cmd_condition(void)
{
    SHELLCMD *t1, *t2;
    t1 = cmd_pipeline();

    while (token == T_AND || token == T_OR) 
    {
        TOKEN savetoken = token;

        t2 = new_shellcmd(token == T_AND ? CMD_AND : CMD_OR);
        t2->left = t1;
        gettoken();
        t2->right = cmd_pipeline();

        if (t2->right == NULL) 
        {
            fprintf(stderr, "command expected after '%s'\n",
                (savetoken == T_AND) ? "&&" : "||");
            nerrors++;
            free_shellcmd(t2);
            interrupt_parsing(0);
        }

        t1 = t2;
    }

    return t1;
}

/**
 * @brief Constructs sequence shellcmds. i.e. ;, &
 * 
 * @return A memory allocated pointer to a shellcmd struct.
 */
static SHELLCMD *cmd_sequence(void)
{
    SHELLCMD *t1, *t2;
    t1 = cmd_condition();

    while ((token == T_SCOLON) || (token == T_BACKGROUND)) 
    {
        t2 = new_shellcmd(token == T_SCOLON ? CMD_SEMICOLON : CMD_BACKGROUND);
        t2->left = t1;
        gettoken();
        t2->right = cmd_condition();
        t1 = t2;
    }

    return t1;
}

/**
 * @brief Constructs the subshell and normal shellcmds. i.e. (), cmd
 * 
 * @return A memory allocated pointer to a shellcmd.
 */
static SHELLCMD *cmd_factor(void)
{
    SHELLCMD *t1, *t2;

    if (token != T_LEFTB)
    {
        t1 = cmd_wordlist();

        if ((t1 != NULL) && (t1->argc == 1) && (token == T_LEFTB))
        {
            t1->left = cmd_factor();
        }
        return t1;
    }

    gettoken();
    t1 = cmd_sequence();

    if (token != T_RIGHTB) 
    {
        fprintf(stderr, "')' expected\n");
        nerrors++;
        free_shellcmd(t1);
        interrupt_parsing(0);
        return NULL;
    }

    if (t1 == NULL) 
    {
        fprintf(stderr, "subshells may not be empty\n");
        nerrors++;
        interrupt_parsing(0);
        return NULL;
    }

    t2 = new_shellcmd(CMD_SUBSHELL);
    t2->left = t1;
    t2->right = NULL;
    gettoken();

    while (is_redirection(token)) 
    {
        if (!get_redirection(t2)) 
        {
 		    free_shellcmd(t2);
 		    interrupt_parsing(0);
 		}

        gettoken();
    }

    t1 = t2;
    return t1;
}

/**
 * @brief Constructs a pipeline shellcmd. i.e. |
 * 
 * @return A memory allocated shellcmd. 
 */
static SHELLCMD *cmd_pipeline(void)
{
    SHELLCMD *t1, *t2;
    t1 = cmd_factor();

    if (token == T_PIPE) 
    {
        if ((t1 != NULL) && (t1->outfile != NULL)) 
        {
            fprintf(stderr, "output cannot be both redirected and piped\n");
            nerrors++;
            free_shellcmd(t1);
            interrupt_parsing(0);
            return NULL;
        }
            
        t2 = new_shellcmd(CMD_PIPE);
        t2->left = t1;
        gettoken();
        t2->right = cmd_pipeline();

        if(t2->right == NULL) 
        {
            fprintf(stderr, "command expected after '|'\n");
            nerrors++;
            free_shellcmd(t2);
            interrupt_parsing(0);
            return NULL;
 	    }

        if ((t2->right != NULL) && (t2->right->infile != NULL)) 
        {
            fprintf(stderr, "input cannot be both redirected and piped\n");
            nerrors++;
            free_shellcmd(t2);
            interrupt_parsing(0);
            return NULL;
        }

        t1 = t2;
    }

    return t1;
}

// -------------------------- parsing stuff -----------------------------

typedef void (*sighandler_t)(int);

#if defined(CGI_INTERFACE)
SHELLCMD *parse_shellcmd_string(char *str)
{
    extern FILE *fp;                        // in parser.c
    extern SHELLCMD	*cmd_sequence(void);	// in parser.c
    SHELLCMD *t1;
    sighandler_t old_handler;
    old_handler = signal(SIGINT, interrupt_parsing);

    if(setjmp(env)) 
    {
        return NULL;
    }

    fp = stdin;
    sprintf(line, "%s\n", str);
    line_length     = (size_t) strlen(line);
    ch_count        = 0;
    init_prompt     = true;
    nerrors	        = 0;
    gettoken();
    t1 = cmd_sequence();

    if (token != T_NEWLINE && token != T_EOF) 
    {
        fprintf(stderr, "garbage at end of line\n");
        nerrors++;
    }

    if (nerrors != 0)
    {
        free_shellcmd(t1);
        t1 = NULL;
    }
    
    return t1;
}

#else

/**
 * @brief Read input from the file pointer to construct a command tree. 
 *  Anticipated for this function to be called from main().
 * 
 * @param fp_   The input file pointer.
 * @return A memory allocated pointer to a shellcmd struct.
 */
SHELLCMD *parse_shellcmd(FILE *fp_)
{
    SHELLCMD *t1;
    sighandler_t old_handler = signal(SIGINT, interrupt_parsing);

    if (setjmp(env)) 
    {
        if(interactive)
        {
            fputc('\n', stdout);
        }
    }

    do 
    {
        t1              = NULL;
        fp              = fp_;
        ch_count        = 0;
        line_length     = 0;
        init_prompt     = true;
        nerrors         = 0;

        if (feof(fp)) 
        {
            break;
        }

        gettoken();
    } 
    while ((t1 = cmd_sequence()) == NULL);

    signal(SIGINT, old_handler);    // control-C to interrupt parsing
    ++prompt_no;

    if ((token != T_NEWLINE) && (token != T_EOF)) 
    {
        fprintf(stderr, "garbage at end of line\n");
        nerrors++;
    }

    if (nerrors != 0)
    {
        free_shellcmd(t1);
        t1 = NULL;
    }
    
    return t1;
}
#endif

/**
 * @brief Free allocated memory for the infile and outfile.
 * 
 * @param t     The shellcmd to free.
 */
static void free_redirection(SHELLCMD *t)
{
    free(t->infile);
    free(t->outfile);
}

/**
 * @brief Deallocate all dynamically allocated memory in a command-tree.
 * Anticipated that this function will be called from main().
 * 
 * @param t     The shellcmd to free.
 */
void free_shellcmd(SHELLCMD *t)
{
    if (t != NULL) 
    {
        switch (t->type) 
        {
        case CMD_COMMAND :
            free_shellcmd(t->left);

            for (int a = 0; a < t->argc; a++)
            {   
                free(t->argv[a]);   // free command-words
            }

            free(t->argv);
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
            free_shellcmd(t->left); // free the subtrees
            free_shellcmd(t->right);
            break;
        }
        
        free(t);    // and free the node itself
    }
}
