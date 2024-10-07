#pragma once
/**
 * @file    hashmap.h
 * @author  Joshua Ng.
 * @brief   Implements a hashmap data container.
 * @date    2023-09-07
 */

#include "iterator.h"
#include "pair.h"
#include <stdbool.h>

/**
 * @brief A node structure for the linked list.
 */
typedef struct NODE
{
    PAIR entry;
    double hash;
} NODE;

/**
 * @brief A hashmap structure for the hash table.
 */
typedef struct HASHMAP
{
    struct NODE *nodes;     // An array of nodes
    size_t size;     // The amount of items in the hashmap.
    size_t capacity; // The number of nodes in the array.
} HASHMAP;

bool            hashmap_resize_capacity     (HASHMAP *hashmap, size_t capacity);
size_t          hashmap_size                (const HASHMAP *hashmap);
bool            hashmap_contains            (const HASHMAP *hashmap, void *key, size_t size);
bool            hashmap_insert              (HASHMAP *hashmap, PAIR entry, PAIR *result);
bool            hashmap_remove              (HASHMAP *hashmap, void *key, size_t size);
void*           hashmap_get                 (HASHMAP *hashmap, void *key, size_t key_size, size_t *value_size);

ITERATOR        hashmap_iterator            (HASHMAP* hashmap);
bool            hashmap_iterator_has_next   (ITERATOR* it);
void*           hashmap_iterator_next       (ITERATOR* it, size_t *size);
