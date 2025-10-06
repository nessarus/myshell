#pragma once
/**
 * @file    set.h
 * @author  Joshua Ng.
 * @brief   Implements a set data container.
 * @date    2023-09-07
 */

#include "iterator.h"
#include <stdbool.h>

struct NODE;

/**
 * @brief A set structure for the hash table.
 */
typedef struct SET
{
    struct NODE *nodes;     // An array of nodes
    size_t size;     // The amount of items in the set.
    size_t capacity; // The number of nodes in the array.
} SET;

SET             set_init                (size_t capacity);
size_t          set_size                (const SET *set);
bool            set_contains            (const SET *set, int data);
bool            set_insert              (SET *set, int data);
bool            set_remove              (SET *set, int data);
void            set_free                (SET *set);

ITERATOR        set_iterator            (SET* set);
bool            set_iterator_has_next   (ITERATOR* it);
void*           set_iterator_next       (ITERATOR* it, size_t *length);
