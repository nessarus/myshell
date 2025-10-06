#pragma once
/**
 * @file    internal.h
 * @author  Joshua Ng
 * @brief   Execute internal commands.
 * @date    2023-08-17
 */

#include "myshell.h"

typedef enum
{
    COMMAND_EXECUTE = 0,
    COMMAND_CD,
    COMMAND_EXIT,
    COMMAND_TIME
} COMMAND;

COMMAND parse_cmd       (char*);
int     exit_shellcmd   (SHELLCMD *, int);
int     cd_shellcmd     (SHELLCMD *);
int     time_shellcmd   (SHELLCMD *);
