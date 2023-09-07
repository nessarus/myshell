#pragma once
/**
 * @file    filepaths.h
 * @author  Joshua Ng
 * @brief   File address path utility functions.
 * @date    2023-08-15
 */

#include <stdbool.h>

char*   join_paths              (const char* path1, const char* path2);
char*   append_filename         (const char* directory, const char* filename);
char*   get_parent_directory    (const char* filepath);
char*   get_filename            (const char* filepath);
char*   normalize_path          (const char* path);
bool    is_absolute_path        (const char* path);
char*   get_absolute_path       (const char* relative_path);
bool    path_exists             (const char* path);
bool    file_exists             (const char* path);
bool    directory_exists        (const char* path);
char*   get_file_extension      (const char* filepath);
