#pragma once
/**
 * @file    set.h
 * @author  Joshua Ng.
 * @brief   Implements a set data container.
 * @date    2023-09-07
 */

#include "iterator.h"
#include <stdbool.h>

typedef struct SET SET;

SET*            set_new                 (void);
size_t          set_size                (const SET *set);
bool            set_contains            (const SET *set, const void *data, size_t length);
bool            set_insert              (SET *set, const void *data, size_t legnth);
bool            set_remove              (SET *set, const void *data, size_t length);
void            set_free                (SET *set);

ITERATOR        set_iterator            (SET* set);
bool            set_iterator_has_next   (ITERATOR* it);
void*           set_iterator_next       (ITERATOR* it, size_t *length);
