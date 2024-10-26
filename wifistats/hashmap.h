#pragma once
/**
 * @file    hashmap.h
 * @author  Joshua Ng.
 * @brief   Implements a hashmap data container.
 * @date    2023-09-07
 */

#include "iterator.h"
#include <stdbool.h>

/**
 * @brief Interface for the hashmap to work with custom container (node).
 */
typedef struct
{
    void*   (*get_key)      (const void *node);
    size_t  (*get_hash)     (const void *node);
    bool    (*equals_key)   (const void *key1, const void *key2);
    size_t  (*hash_key)     (const void *key);
} HashMapInterface;

/**
 * @brief A hashmap structure for the hash table.
 */
typedef struct HashMap
{
    HashMapInterface interface;     // Hashing customisations.
    void **nodes;                    // An array of nodes
    size_t size;        // The amount of items in the hashmap.
    size_t capacity;    // The number of nodes in the array.
} HashMap;

void** hashmap_resize(HashMap *hashmap, void *nodes[], size_t capacity);
bool hashmap_contains(const HashMap *hashmap, const void *key);

typedef struct HashMapResult
{
    void *node;
    bool success;
} HashMapResult;

HashMapResult   hashmap_insert      (HashMap *hashmap, void *node);
HashMapResult   hashmap_remove      (HashMap *hashmap, const void *key);
void*           hashmap_get         (const HashMap *hashmap, const void *key);

ITERATOR        hashmap_iterator            (const HashMap* hashmap, size_t index);
bool            hashmap_iterator_has_next   (ITERATOR* it);
void*           hashmap_iterator_next       (ITERATOR* it);
