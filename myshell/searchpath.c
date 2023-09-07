/**
 * @file        searchpath.c
 * @author      Joshua Ng
 * @brief       Searches a list of directories for a file.
 * @date        2023-08-14
 */

#if defined(__linux__)
    char *strdup(const char *str);
#endif

#include "searchpath.h"
#include "myshell.h"
#include "globals.h"
#include "filepaths.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * @brief Search the directories in the path list for the requested file.
 * 
 * @param pathlist  A string colon separated list of directory names.
 * @param file      The file to search for.
 * @return The path to the requested file if found else NULL.
 */
char* searchpath(const char* pathlist, const char* file)
{
    char *path = NULL;
    char *pathlist_copy = strdup(pathlist);
    check_allocation(pathlist_copy);
    char *directory = strtok(pathlist_copy, COLON);

    while (directory)
    {
        path = append_filename(directory, file);
        check_allocation(path);
        
        if (path_exists(path))
        {
            break;
        }

        free(path);
        path = NULL;
        directory = strtok(NULL, COLON);
    }

    free(pathlist_copy);
    return path;
}
