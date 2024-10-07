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
    size_t count;
    bool (*has_next)(struct ITERATOR *it);
    void* (*next)(struct ITERATOR *it, size_t *size);
} ITERATOR;

bool iterator_has_next(ITERATOR *it);
void* iterator_next(ITERATOR *it, size_t *size);
