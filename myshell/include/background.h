#pragma once
/**
 * @file    background.c
 * @author  Joshua Ng
 * @brief   Executes shell commands asynchronously.
 * @date    2023-09-08
 */

#include "myshell.h"

int background_shellcmd(SHELLCMD *t);
void background_exit(void);
