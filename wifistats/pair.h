/**
 * @file pair.h
 * @author  Joshua Ng.
 * @brief   Implements a pair data container.
 * @date 2024-09-23
 */

#include <stddef.h>

typedef struct PAIR 
{
    void *first;
    size_t first_size;
    void *second;
    size_t second_size;
} PAIR;

