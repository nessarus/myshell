#pragma once
/**
 * @file    iterator.h
 * @author  Joshua Ng.
 * @brief   Implements an iterator.
 * @date    2024-08-25
 */

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Define an iterator structure for the hashmap
 */
typedef struct ITERATOR
{
    void *container;
    size_t index;
    bool (*has_next)(struct ITERATOR *it);
    void* (*next)(struct ITERATOR *it);
} ITERATOR;

bool iterator_has_next(ITERATOR *it);
void* iterator_next(ITERATOR *it);
