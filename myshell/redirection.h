#pragma once
/**
 * @file    redirection.c
 * @author  Joshua Ng
 * @brief   Put the input or output to a given file.
 * @date    2023-09-08
 */

#include "myshell.h"

struct REDIRECTION;

int redirection(char* file, int flags, int fd_old);
struct REDIRECTION* redirection_shellcmd(SHELLCMD *t);
void free_redirection_shellcmd(SHELLCMD *t, struct REDIRECTION *r);
