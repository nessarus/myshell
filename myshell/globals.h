#pragma once
/**
 * @file    globals.c
 * @author  Joshua Ng
 * @brief   Global variables.
 * @date    2023-08-18
 */

#include "myshell.h"

/**
 * @brief Macro to ensure that pointer is not null.
 */
#define check_allocation(p) \
    check_allocation0(p, __FILE__, __func__, __LINE__)
void check_allocation0(void *p, char *file, const char *func, int line);

/**
 * @brief Macro to print the requested command-tree.
 */
#define print_shellcmd(t) \
    printf("called from %s, %s() line %i:\n", __FILE__,__func__,__LINE__); \
    print_shellcmd0(t)
void print_shellcmd0(SHELLCMD *t);

/**
 * @brief Macro to ensure function has not returned an error.
 * @param result  The function result.
 */
#define check_error(result) \
    check_error0(result, __FILE__, __func__, __LINE__)
void check_error0(int result, char *file, const char *func, int line);


void print_command_error(char *file, char *argv);
