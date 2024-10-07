/**
 * @file    hashmap.h
 * @author  Joshua Ng.
 * @brief   Implements a hashmap data container.
 * @date    2023-09-07
 */

#include "hashmap.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GOLDEN_RATIO 0.6180339887

/**
 * @brief A function that scrambles the data into a hash.
 *
 * @param data      The data to be hashed.
 * @param size      The size of data in bytes.
 * @return Return's a double between 0 and 1.
 */
static double hashdata(void *data, size_t size)
{
    // Prime number for combining hash values
    const uint8_t prime = 101;
    size_t value = 0;
    const uint8_t *const hex = (const uint8_t *const)data;

    for (size_t i = 0; i < size; i++)
    {
        value = value * prime * hex[i];
    }

    return fmod(value * GOLDEN_RATIO, 1.0);
}

/**
 * @brief Get the size of the hashmap.
 *
 * @param hashmap   The hashmap data structure.
 * @return The amount of items in the hashmap.
 */
size_t hashmap_size(const HASHMAP *hashmap)
{
    return hashmap->size;
}

/**
 * @brief A function that checks if an key is in the hashmap.
 *
 * @param hashmap   The hashmap data structure.
 * @param key       The key to check.
 * @param
 * @return True if element is in the hashmap and false otherwise.
 */
bool hashmap_contains(const HASHMAP *hashmap, void *key, size_t size)
{
    const double hash = hashdata(key, size);
    const size_t i = (size_t)(hash * hashmap->capacity);

    for (int j = 0; j < hashmap->capacity; j++)
    {
        const NODE *const node = hashmap->nodes + ((i + j) % hashmap->capacity);

        if (node->entry.first == NULL)
        {
            break;
        }

        if ((node->hash == hash) &&
            (node->entry.first_size == size) &&
            memcmp(node->entry.first, key, size) == 0)
        {
            return true;
        }
    }

    return false;
}

/**
 * @brief Helper function to insert a node into the hashmap.
 *
 * @param hashmap   A pointer to a hashmap.
 * @param node  The node to insert.
 */
static void hashmap_insert_node(HASHMAP *hashmap, const NODE *node)
{
    const size_t i = (size_t)(node->hash * hashmap->capacity);

    for (int j = 0; j < hashmap->capacity; j++)
    {
        NODE *const temp = hashmap->nodes + ((i + j) % hashmap->capacity);

        if (temp->entry.first == NULL)
        {
            *temp = *node;
            return;
        }
    }
}

/**
 * @brief A helper function to resize the capacity, the array size, of the hashmap.
 *
 * @param hashmap   A pointer to a hashmap.
 * @param capacity  Size of capacity to resize to.
 * @return true     Capacity resize success.
 * @return false    Capacity resize failure.
 */
bool hashmap_resize_capacity(HASHMAP *hashmap, size_t capacity)
{
    HASHMAP saved = *hashmap;
    hashmap->capacity = capacity;
    hashmap->nodes = calloc(capacity, sizeof(NODE));

    if (!hashmap->nodes)
    {
        *hashmap = saved;
        return false;
    }

    for (size_t i = 0; i < saved.capacity; i++)
    {
        const NODE *const node = saved.nodes + i;

        if (node->entry.first != NULL)
        {
            hashmap_insert_node(hashmap, node);
        }
    }

    free(saved.nodes);
    return true;
}

/**
 * @brief A function that inserts key value pair into the hashmap if it is not
 * already present and returns if successful or not.
 *
 * @param hashmap   The hashmap data structure.
 * @param entry     A pair of key and value.
 * @param result    A pointer to the stored pair with the newly inserted
 *  element or one that has teh equivalent key in the map. 
 * @return True if the data was inserted successfully and false otherwise.
 */
bool hashmap_insert(HASHMAP *hashmap, PAIR entry, PAIR *result)
{
    const double hash = hashdata(entry.first, entry.first_size);
    NODE node = {.entry = entry, .hash = hash};
    size_t i = (size_t)(hash * hashmap->capacity);
    size_t j;

    for (j = 0; j < hashmap->capacity; j++)
    {
        NODE *const temp = hashmap->nodes + (i + j) % hashmap->capacity;

        if (temp->entry.first == NULL)
        {
            *temp = node;
            hashmap->size++;
            if (result)
            {
                *result = node.entry;
            }
            break;
        }

        if 
        (
            (temp->hash == hash) &&
            (temp->entry.first_size == entry.first_size) &&
            (memcmp(temp->entry.first, entry.first, entry.first_size) == 0)
        )
        {
            if (result)
            {
                *result = temp->entry;
            }
            return false;
        }
    }

    if (j == hashmap->capacity)
    {
        hashmap_insert_node(hashmap, &node);
        hashmap->size++;
    }

    return true;
}

/**
 * @brief Looks up the value of the provided key.
 * 
 * @param hashmap   The hashmap data structure.
 * @param key       The key to lookup.
 * @param key_size  The size of the key.
 * @param value_size    Outputs the size of the value.
 * @return void*    A pointer to the value.
 */
void *hashmap_get(HASHMAP *hashmap,
                  void *key,
                  size_t key_size,
                  size_t *value_size)
{
    const double hash = hashdata(key, key_size);
    const size_t i = (size_t)(hash * hashmap->capacity);

    for (size_t j = 0; j < hashmap->capacity; j++)
    {
        NODE *const node = hashmap->nodes + ((i + j) % hashmap->capacity);

        if (node->entry.first == NULL)
        {
            break;
        }

        if ((node->hash == hash) &&
            (node->entry.first_size == key_size) &&
            (memcmp(node->entry.first, key, key_size) == 0))
        {
            if (value_size)
            {
                *value_size = node->entry.second_size;
            }
            return node->entry.second;
        }
    }

    return NULL;
}

/**
 * @brief A helper function to remove the gap made by
 *  removing an element.
 *
 * @param hashmap   The hashmap to clean up.
 * @param index The index of the removed element.
 */
static void hashmap_cleanup(const HASHMAP *hashmap, size_t index)
{
    NODE *empty = hashmap->nodes + index;
    size_t iempty = index;

    for (size_t j = 1; j < hashmap->capacity; j++)
    {
        const size_t i = ((index + j) % hashmap->capacity);
        NODE *const node = hashmap->nodes + i;

        if (node->entry.first == NULL)
        {
            break;
        }

        const size_t ihash = (size_t)(node->hash * hashmap->capacity);
        if ((ihash <= iempty) && ((i > iempty) || (ihash > i)))
        {
            *empty = *node;
            empty = node;
            *empty = (NODE){0};
            iempty = i;
        }
    }
}

/**
 * @brief A function that removes an element from the hashmap if it is present
 * and returns if successful or not.
 *
 * @param hashmap       The hashmap data structure.
 * @param key           The key to remove.
 * @param size          The size of the key.
 * @return True if the value was successfully removed and false otherwise.
 */
bool hashmap_remove(HASHMAP *hashmap, void *key, size_t size)
{
    const double hash = hashdata(key, size);
    const size_t i = (size_t)(hash * hashmap->capacity);
    size_t j;
    size_t inode = i;

    for (j = 0; j < hashmap->capacity; j++)
    {
        inode = (i + j) % hashmap->capacity;
        NODE *const node = hashmap->nodes + inode;

        if (node->entry.first == NULL)
        {
            return false;
        }

        if ((node->hash == hash) &&
            (node->entry.first_size == size) &&
            (memcmp(node->entry.first, key, size) == 0)
        )
        {
            *node = (NODE){0};
            hashmap->size--;
            break;
        }
    }

    if (j == hashmap->capacity)
    {
        return false;
    }

    hashmap_cleanup(hashmap, inode);

    return true;
}

/**
 * @brief Initialize an iterator for the hashmap.
 *
 * @param hashmap   The pointer to the hashmap
 * @return An iterator.
 */
ITERATOR hashmap_iterator(HASHMAP *hashmap)
{
    return (ITERATOR){
        .container = (void *)hashmap,
        .index = 0,
        .has_next = &hashmap_iterator_has_next,
        .next = &hashmap_iterator_next};
}

/**
 * @brief Check if the iterator has more elements to iterate over.
 *
 * @param it    The iterator.
 * @return Return true if the current iterator node is valid.
 */
bool hashmap_iterator_has_next(ITERATOR *it)
{
    return (it->count < ((HASHMAP *)it->container)->size);
}

/**
 * @brief Advance the iterator to the next element in the hashmap.
 *
 * @param it        The iterator.
 * @param length    A pointer to return the element's data length.
 * @return Returns a pointer to a pair of key and value.
 */
void *hashmap_iterator_next(ITERATOR *it, size_t *length)
{
    void *data = NULL;
    const HASHMAP *const hashmap = (HASHMAP *)it->container;
    for (size_t i = it->index; i < hashmap->capacity; i++)
    {
        const NODE *const node = hashmap->nodes + i;
        if (node->entry.first)
        {
            data = (void *) &node->entry;
            if (length)
            {
                *length = sizeof(node->entry);
            }
            it->index = i + 1;
            it->count++;
            break;
        }
    }

    return data;
}
