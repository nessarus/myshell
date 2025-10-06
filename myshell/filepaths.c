/**
 * @file    filepaths.c
 * @author  Joshua Ng
 * @brief   File address path utility functions.
 * @date    2023-08-15
 */

#include "filepaths.h"
#include "myshell.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/stat.h>

/**
 * @brief Joins two path components.
 * 
 * @param path1     Left path.
 * @param path2     Right path.
 * @return Returns a memory allocated char array.
 */
char* join_paths(const char* path1, const char* path2) 
{
    // +2 for the separator and null terminator
    char* result = (char*) malloc(strlen(path1) + strlen(path2) + 2);
    check_allocation(result);

    strcpy(result, path1);
    size_t length = strlen(result);
    if ((length > 0) && (result[length - 1] != '/')) 
    {
        strcat(result, "/");
    }

    strcat(result, path2);

    return result;
}

/**
 * @brief Append a filename to a directory path.
 * 
 * @param directory     Directory path.
 * @param filename      Filename.
 * @return A memory allocated char array.
 */
char* append_filename(const char* directory, const char* filename) 
{
    return join_paths(directory, filename);
}

/**
 * @brief Get the parent directory path from a file path.
 * 
 * @param filepath  The file path.
 * @return A memory allocated char array.
 */
char* get_parent_directory(const char* filepath) 
{
    char* path = strdup(filepath);
    check_allocation(path);

    char* parent = strdup(dirname(path));
    check_allocation(parent);

    free(path);
    return parent;
}

/**
 * @brief Get the filename from a file path.
 * 
 * @param filepath  The file path.
 * @return A memory allocated char array.
 */
char *get_filename(const char *filepath)
{
    char* path = strdup(filepath);
    check_allocation(path);

    char* name = strdup(basename(path));
    check_allocation(name);

    free(path);
    return name;
}

/**
 * @brief Normalize a given path by resolving "/", ".." and "." to produce 
 * an absolute path.
 * 
 * @param path  A char pointer to the path.
 * @return A memory allocated char array.
 */
char* normalize_path(const char* path) 
{
    char* normalized = realpath(path, NULL);
    return normalized;
}

/**
 * @brief Check if a path is absolute.
 * 
 * @param path  A char pointer to the path.
 * @return Path is absolute.
 */
bool is_absolute_path(const char* path) 
{
    return (path[0] == '/');
}

/**
 * @brief Get the absolute path.
 * 
 * @param path  The path to lookup.
 * @return A memory allocated char array.
 */
char* get_absolute_path(const char* path) 
{
    char* absolute = realpath(path, NULL);
    return absolute;
}

/**
 * @brief Check if a path exists.
 * 
 * @param path  Path address.
 * @return True if path exists.
 */
bool path_exists(const char* path) 
{
    return access(path, F_OK) == 0;
}

/**
 * @brief Check if a file exists.
 * 
 * @param filepath  File address path.
 * @return True if file exists.
 */
bool file_exists(const char* path) 
{
    struct stat info;
    if (stat(path, &info) == 0) 
    {
        return S_ISREG(info.st_mode);
    }
    return false;
}

/**
 * @brief Check if a directory exists.
 * 
 * @param path  A char pointer to the path.
 * @return True if the directory exists.
 */
bool directory_exists(const char* path) 
{
    struct stat info;
    if (stat(path, &info) == 0) 
    {
        return S_ISDIR(info.st_mode);
    }
    return false;
}

/**
 * @brief Get the file extension object
 * 
 * @param filepath  A char pointer to the path.
 * @return A memory allocated char array. NULL if not found.
 */
char* get_file_extension(const char* filepath) 
{
    char* dot = strrchr(filepath, '.');

    if ((dot != NULL) && (dot != filepath) && (dot[1] != '\0')) 
    {
        char* extension = strdup(dot + 1);
        check_allocation(extension);
        return extension;
    }

    return NULL;
}
