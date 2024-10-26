/**
 * @file    hashmap.h
 * @author  Joshua Ng.
 * @brief   Implements a hashmap data container.
 * @date    2023-09-07
 */

#include "hashmap.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Checks if a node with the given key is in the hashmap.
 *
 * @param hashmap  The hashmap data structure.
 * @param key           The key to check.
 * @return True if element is in the hashmap and false otherwise.
 */
bool hashmap_contains(const HashMap *hashmap, const void *key)
{
    const size_t hash = hashmap->interface.hash_key(key);
    const size_t i = hash % hashmap->capacity;

    for (int j = 0; j < hashmap->capacity; j++)
    {
        const void *node = hashmap->nodes[(i + j) % hashmap->capacity];

        if (node == NULL)
        {
            break;
        }
        
        const void *node_key = hashmap->interface.get_key(node);
        if (hashmap->interface.equals_key(key, node_key))
        {
            return true;
        }
    }

    return false;
}

/**
 * @brief A helper function to insert a node into the hashmap.
 *
 * @param hashmap   A pointer to a hashmap.
 * @param node  The node to insert.
 */
static void hashmap_insert_node(const HashMap *hashmap, void *node)
{
    const size_t i = hashmap->interface.get_hash(node) % hashmap->capacity;

    for (size_t j = 0; j < hashmap->capacity; j++)
    {
        const size_t index = ((i + j) % hashmap->capacity);
        void **const map_node = hashmap->nodes + index;

        if (*map_node == NULL)
        {
            *map_node = node;
            return;
        }
    }
}

/**
 * @brief Changes the node array with a new one.
 *
 * @param hashmap        A pointer to a hashmap.
 * @param nodes             A node array.
 * @param capacity      The size of the new node array.
 * @return A pointer to the old node array.
 */
void **hashmap_resize(HashMap *hashmap, void *nodes[], size_t capacity)
{
    HashMap saved = *hashmap;
    hashmap->capacity = capacity;
    hashmap->nodes = nodes;

    for (size_t i = 0; i < saved.capacity; i++)
    {
        void *const node = saved.nodes[i];

        if (node != NULL)
        {
            hashmap_insert_node(hashmap, node);
        }
    }
    
    return saved.nodes;
}

/**
 * @brief A function that inserts a container with a key into the hashmap if it
 * is not already present.
 *
 * @param hashmap   The hashmap data structure.
 * @param node A container with a key.
 * @return A struct with a pointer to newly inserted node or one with the
 * equivalent key in the map, and a boolean if node was actually inserted.
 */
HashMapResult hashmap_insert(HashMap *hashmap, void *node)
{
    HashMapResult result = {0};
    const void *const key = hashmap->interface.get_key(node);
    const size_t i = hashmap->interface.get_hash(node) % hashmap->capacity;

    for (size_t j = 0; j < hashmap->capacity; j++)
    {
        const size_t index = (i + j) % hashmap->capacity;
        void **const map_node = hashmap->nodes + index;

        if (*map_node == NULL)
        {
            *map_node = node;
            hashmap->size++;
            result = (HashMapResult){.node = node, .success = true};
            break;
        }

        const void *const key2 = hashmap->interface.get_key(*map_node);
        if (hashmap->interface.equals_key(key, key2))
        {
            result.node = *map_node;
            break;
        }
    }

    return result;
}

/**
 * @brief Looks up the value of the provided key.
 * 
 * @param hashmap   The hashmap data structure.
 * @param key       The key to lookup.
 * @return void*    A pointer to the value.
 */
void *hashmap_get(const HashMap *hashmap, const void *key)
{
    const size_t i = hashmap->interface.hash_key(key) % hashmap->capacity;

    for (size_t j = 0; j < hashmap->capacity; j++)
    {
        const size_t index = (i + j) % hashmap->capacity;
        void *node = hashmap->nodes[index];

        if (node == NULL)
        {
            break;
        }

        const void *const key2 = hashmap->interface.get_key(node);
        if (hashmap->interface.equals_key(key, key2))
        {
            return node;
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
static void hashmap_cleanup(const HashMap *hashmap, size_t index)
{
    void **empty = hashmap->nodes + index;
    size_t iempty = index;

    for (size_t j = 1; j < hashmap->capacity; j++)
    {
        const size_t i = ((index + j) % hashmap->capacity);
        void **const node = hashmap->nodes + i;

        if (*node == NULL)
        {
            break;
        }

        const size_t hash = hashmap->interface.get_hash(*node);
        const size_t ihash = hash % hashmap->capacity;
        if ((ihash <= iempty) && ((i > iempty) || (ihash > i)))
        {
            *empty = *node;
            empty = node;
            *empty = NULL;
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
 * @return True if the value was successfully removed and false otherwise.
 */
HashMapResult hashmap_remove(HashMap *hashmap, const void *key)
{
    HashMapResult result = {0};
    const size_t i = hashmap->interface.hash_key(key) % hashmap->capacity;
    size_t inode = i;

    for (size_t j = 0; j < hashmap->capacity; j++)
    {
        inode = (i + j) % hashmap->capacity;
        void **const node = hashmap->nodes + inode;

        if (*node == NULL)
        {
            break;
        }
        
        const void *const key2 = hashmap->interface.get_key(*node);
        if (hashmap->interface.equals_key(key, key2))
        {
            result = (HashMapResult){.node = *node, .success = true};
            *node = NULL;
            hashmap->size--;
            break;
        }
    }

    if (result.success)
    {
        hashmap_cleanup(hashmap, inode);
    }


    return result;
}

/**
 * @brief Initialize an iterator for the hashmap.
 *
 * @param hashmap   The pointer to the hashmap
 * @return An iterator.
 */
ITERATOR hashmap_iterator(const HashMap *hashmap, size_t index)
{
    return (ITERATOR){
        .container = (void *)hashmap,
        .index = index,
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
    const HashMap *const hashmap = (HashMap *)it->container;
    for (size_t i = it->index; i < hashmap->capacity; i++)
    {
        if (hashmap->nodes[i] != NULL)
        {
            it->index = i;
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Advance the iterator to the next element in the hashmap.
 *
 * @param it        The iterator.
 * @param length    A pointer to return the element's data length.
 * @return Returns a pointer to a pair of key and value.
 */
void *hashmap_iterator_next(ITERATOR *it)
{
    void *node = NULL;
    const HashMap *const hashmap = (HashMap *)it->container;
    for (size_t i = it->index; i < hashmap->capacity; i++)
    {
        node = hashmap->nodes[i];
        if (node != NULL)
        {
            it->index = i + 1;
            break;
        }
    }

    return node;
}
