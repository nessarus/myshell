#pragma once
/**
 * @file    set.h
 * @author  Joshua Ng.
 * @brief   Implements a set data container.
 * @date    2023-09-07
 */

#include "stdbool.h"

struct SET;

struct SET*     set_create              (void);
int             set_size                (struct SET *set);
bool            set_contains            (struct SET *set, int value);
bool            set_insert              (struct SET *set, int value);
bool            set_remove              (struct SET *set, int value);
void            set_free                (struct SET *set);

struct SET_ITERATOR;

struct SET_ITERATOR* set_iterator_create(struct SET* set);
void            set_iterator_next       (struct SET_ITERATOR* it);
bool            set_iterator_has_next   (struct SET_ITERATOR* it);
int             set_iterator_get        (struct SET_ITERATOR* it);
void            set_iterator_free       (struct SET_ITERATOR* it);
