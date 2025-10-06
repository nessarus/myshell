#pragma once

/**
 * @file    parser.h
 * @author  Joshua Ng
 * @brief   Parses the shellcmd.
 * @date    2023-08-28
 */

#include "myshell.h"

SHELLCMD *parse_shellcmd(FILE *_fp);
void free_shellcmd(SHELLCMD *t);
