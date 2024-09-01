/**
 * @file    set.h
 * @author  Joshua Ng.
 * @brief   Implements a set data container.
 * @date    2023-09-07
 */

#include "set.h"
#include "globals.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GOLDEN_RATIO 0.6180339887
#define MINIMUM_LOAD 2
#define MAXIMUM_LOAD 8

/**
 * @brief A node structure for the linked list.
 */
typedef struct NODE
{
    void *data; // the element stored in the node
    size_t length;
    double hash;
} NODE;

/**
 * @brief A set structure for the hash table.
 */
struct SET
{
    NODE *nodes; // An array of nodes
    size_t size; // The amount of items in the set.
    size_t capacity;    // The number of nodes in the array.
};

/**
 * @brief A function that scrambles the data into a hash.
 *
 * @param data      The data to be hashed.
 * @param length    the length of the data.
 * @return Return's a double between 0 and 1.
 */
static double hashdata(const void *data, size_t length)
{
    // Prime number for combining hash values
    const uint8_t prime = 101;
    size_t value = 0;
    const uint8_t *const hex = (const uint8_t *const)data;

    for (size_t i = 0; i < length; i++)
    {
        value = value * prime * hex[i];
    }

    return fmod(value * GOLDEN_RATIO, 1.0);
}

/**
 * @brief A function that creates a new set and returns its pointer.
 *
 * @return A memory allocated set.
 */
SET* set_new(void)
{
    SET *set = (SET *) malloc(sizeof(SET));
    *set = (SET){.nodes = calloc(1, sizeof(NODE)), .size = 0, .capacity = 1};
    return set;
}

/**
 * @brief Get the size of the set.
 *
 * @param set   The set data structure.
 * @return The amount of items in the set.
 */
size_t set_size(const SET *set)
{
    return set->size;
}

/**
 * @brief A function that checks if an element is in the set.
 *
 * @param set       The set data structure.
 * @param data      The data to check.
 * @param length    The length of the data.
 * @return True if element is in the set and false otherwise.
 */
bool set_contains(const SET *set, const void *data, size_t length)
{
    const double hash = hashdata(data, length);
    const size_t i = (size_t)(hash * set->capacity);

    for (int j = 0; j < set->capacity; j++)
    {
        const NODE *const node = set->nodes + ((i + j) % set->capacity);

        if (node->data == NULL)
        {
            break;
        }

        if ((node->hash == hash) &&
            (node->length == length) &&
            (memcmp(node->data, data, length) == 0))
        {
            return true;
        }
    }

    return false;
}

/**
 * @brief Helper function to insert a node into the set.
 * 
 * @param set   A pointer to a set.
 * @param node  The node to insert.
 */
static void set_insert_node(SET *set, const NODE *node)
{
    const size_t i = (size_t)(node->hash * set->capacity);

    for (int j = 0; j < set->capacity; j++)
    {
        NODE *const temp = set->nodes + ((i + j) % set->capacity);

        if (temp->data == NULL)
        {
            *temp = *node;
            return;
        }
    }
}

/**
 * @brief Resize the capacity, the array size, of the set.
 *
 * @param set       A pointer to a set.
 * @param capacity  The new set capacity.
 * @return true     Capacity resize success.
 * @return false    Capacity resize failure.
 */
static bool set_resize_capacity(SET *set, size_t capacity)
{
    SET saved = *set;
    set->capacity = capacity;
    if (set->capacity < MINIMUM_LOAD)
    {
        set->capacity = MINIMUM_LOAD;
    }
    set->nodes = calloc(set->capacity, sizeof(NODE));

    if (!set->nodes)
    {
        *set = saved;
        return false;
    }

    for (int i = 0; i < saved.capacity; i++)
    {
        const NODE *const node = saved.nodes + i;

        if (node->data)
        {
            set_insert_node(set, node);
        }
    }

    free(saved.nodes);
    return true;
}

/**
 * @brief A function that inserts an element into the set if it is not
 * already present and returns if successful or not.
 *
 * @param set   The set data structure.
 * @param value The value to insert.
 * @return True if the value was inserted successfully and false otherwise.
 */
bool set_insert(SET *set, const void *data, size_t length)
{
    const double hash = hashdata(data, length);
    size_t i = (size_t)(hash * set->capacity);

    for (int j = 0; j < set->capacity; j++)
    {
        NODE *node = set->nodes + (i + j) % set->capacity;

        if (!node->data)
        {
            break;
        }

        if ((node->hash == hash) &&
            (node->length == length) &&
            (memcmp(node->data, data, length) == 0))
        {
            return false;
        }
    }

    NODE node = {.data = malloc(length), .length = length, .hash = hash};
    if (!node.data)
    {
        return false;
    }

    if ((set->size * MINIMUM_LOAD) >= set->capacity)
    {
        if (!set_resize_capacity(set, set->capacity * MINIMUM_LOAD))
        {
            free(node.data);
            return false;
        }
    }

    memcpy(node.data, data, length);
    set_insert_node(set, &node);
    set->size++;
    return true;
}

/**
 * @brief A helper function to remove the gap made by 
 *  removing an element.
 * 
 * @param set   The set to clean up.
 * @param index The index of the removed element.
 */
static void set_cleanup(const SET *set, size_t index)
{
    NODE *empty = set->nodes + index;
    size_t iempty = index;

    for (size_t j = 1; j < set->capacity; j++)
    {
        const size_t i = ((index + j) % set->capacity);
        NODE *const node = set->nodes + i;

        if (!node->data)
        {
            break;
        }

        const size_t ihash = (size_t)(node->hash * set->capacity);
        if ((ihash <= iempty) && ((i > iempty) || (ihash > i)))
        {
            *empty = *node;
            empty = node;
            empty->data = NULL;
            iempty = i;
        }
    }
}

/**
 * @brief A function that removes an element from the set if it is present
 * and returns if successful or not.
 *
 * @param set       The set data structure.
 * @param data      The data to remove.
 * @param length    The length of the data.
 * @return True if the value was successfully removed and false otherwise.
 */
bool set_remove(SET *set, const void *data, size_t length)
{
    const double hash = hashdata(data, length);
    const size_t ihash = (size_t)(hash * set->capacity);
    NODE *node = NULL;
    size_t i = ihash;

    for (int j = 0; j < set->capacity; j++)
    {
        i = (ihash + j) % set->capacity;
        NODE *const temp = set->nodes + i;

        if (!temp->data)
        {
            return false;
        }

        if ((temp->hash == hash) &&
            (temp->length == length) &&
            (memcmp(temp->data, data, length) == 0))
        {
            node = temp;
            break;
        }
    }

    if (node)
    {
        free(node->data);
        node->data = NULL;
        set->size--;

        if ((set->size * MAXIMUM_LOAD) <= set->capacity)
        {
            if (!set_resize_capacity(set, set->capacity/2))
            {
                return false;
            }
        }
        else
        {
            set_cleanup(set, i);
        }

        return true;
    }

    return false;
}

/**
 * @brief A function that frees all the memory allocated for the set and its
 * nodes.
 *
 * @param set   The set data structure.
 */
void set_free(SET *set)
{
    ITERATOR it = set_iterator(set);
    while (iterator_has_next(&it))
    {
        free(iterator_next(&it, NULL));
    }
    free(set->nodes);
    free(set);
}

/**
 * @brief Initialize an iterator for the hashmap.
 *
 * @param set   The pointer to the hashmap
 * @return An iterator.
 */
ITERATOR set_iterator(SET *set)
{
    return (ITERATOR){
        .container = (void *)set,
        .index = 0,
        .has_next = &set_iterator_has_next,
        .next = &set_iterator_next};
}

/**
 * @brief Check if the iterator has more elements to iterate over.
 *
 * @param it    The iterator.
 * @return Return true if the current iterator node is valid.
 */
bool set_iterator_has_next(ITERATOR *it)
{
    return (it->count < ((SET *)it->container)->size);
}

/**
 * @brief Advance the iterator to the next element in the set.
 *
 * @param it        The iterator.
 * @param length    A pointer to return the element's data length.
 * @return Returns a pointer to the element's data.
 */
void *set_iterator_next(ITERATOR *it, size_t *length)
{
    void *data = NULL;
    SET *const set = (SET *) it->container;
    for (size_t i = it->index; i < set->capacity; i++)
    {
        if (set->nodes[i].data)
        {
            data = set->nodes[i].data;
            if (length)
            {
                *length = set->nodes[i].length;
            }
            it->index = i + 1;
            it->count++;
            break;
        }
    }

    return data;
}
