/**
 * @file    set.h
 * @author  Joshua Ng.
 * @brief   Implements a set data container.
 * @date    2023-09-07
 */

#include "set.h"
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
    int data; // the element stored in the node
    bool has_data;
    double hash;
} NODE;

/**
 * @brief A function that scrambles the data into a hash.
 *
 * @param data      The data to be hashed.
 * @return Return's a double between 0 and 1.
 */
static double hashdata(int data)
{
    return fmod(((double) data) * GOLDEN_RATIO, 1.0);
}

/**
 * @brief A function that creates a new set and returns its pointer.
 * @param capacity      Initalize capacity of the set.
 * @return A memory allocated set.
 */
SET set_init(size_t capacity)
{
    SET set = {.nodes = calloc(capacity, sizeof(NODE))};
    if (set.nodes)
    {
        set.capacity = capacity;
    }
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
 * @return True if element is in the set and false otherwise.
 */
bool set_contains(const SET *set, int data)
{
    const double hash = hashdata(data);
    const size_t i = (size_t)(hash * set->capacity);

    for (int j = 0; j < set->capacity; j++)
    {
        const NODE *const node = set->nodes + ((i + j) % set->capacity);

        if (!node->has_data)
        {
            break;
        }

        if (node->data == data)
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

        if (!temp->has_data)
        {
            *temp = *node;
            return;
        }
    }
}

/**
 * @brief A helper function to resize the capacity, the array size, of the set.
 *
 * @param set       A pointer to a set.
 * @param capacity  Size of capacity to resize to.
 * @return true     Capacity resize success.
 * @return false    Capacity resize failure.
 */
static bool set_resize_capacity(SET *set, size_t capacity)
{
    SET saved = *set;
    set->capacity = capacity;
    set->nodes = calloc(capacity, sizeof(NODE));

    if (!set->nodes)
    {
        *set = saved;
        return false;
    }

    for (int i = 0; i < saved.capacity; i++)
    {
        const NODE *const node = saved.nodes + i;

        if (node->has_data)
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
 * @param data  The data to insert.
 * @return True if the data was inserted successfully and false otherwise.
 */
bool set_insert(SET *set, int data)
{
    const double hash = hashdata(data);
    NODE node = {.data = data, .hash = hash, .has_data = true};
    size_t i = (size_t)(hash * set->capacity);
    size_t j;

    for (j = 0; j < set->capacity; j++)
    {
        NODE *const temp = set->nodes + (i + j) % set->capacity;

        if (!temp->has_data)
        {
            *temp = node;
            set->size++;
            break;
        }

        if (temp->data == data)
        {
            return false;
        }
    }

    if (j == set->capacity)
    {
        j += !set->capacity;
        if (!set_resize_capacity(set, j * MINIMUM_LOAD))
        {
            return false;
        }

        set_insert_node(set, &node);
        set->size++;
    }

    if ((set->size * MINIMUM_LOAD) > set->capacity)
    {
        set_resize_capacity(set, set->capacity * MINIMUM_LOAD);
    }

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

        if (!node->has_data)
        {
            break;
        }

        const size_t ihash = (size_t)(node->hash * set->capacity);
        if ((ihash <= iempty) && ((i > iempty) || (ihash > i)))
        {
            *empty = *node;
            empty = node;
            empty->has_data = false;
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
 * @return True if the value was successfully removed and false otherwise.
 */
bool set_remove(SET *set, int data)
{
    const double hash = hashdata(data);
    const size_t i = (size_t)(hash * set->capacity);
    size_t j;
    size_t inode = i;

    for (j = 0; j < set->capacity; j++)
    {
        inode = (i + j) % set->capacity;
        NODE *const node = set->nodes + inode;

        if (!node->has_data)
        {
            return false;
        }

        if (node->data == data)
        {
            *node = (NODE){0};
            set->size--;
            break;
        }
    }

    if (j == set->capacity)
    {
        return false;
    }

    if ((set->size * MAXIMUM_LOAD) <= set->capacity)
    {
        if (!set_resize_capacity(set, set->capacity / MINIMUM_LOAD))
        {
            return false;
        }
    }
    else
    {
        set_cleanup(set, inode);
    }

    return true;
}

/**
 * @brief A function that frees all the memory allocated for the set and its
 * nodes.
 *
 * @param set   The set data structure.
 */
void set_free(SET *set)
{
    free(set->nodes);
    *set = (SET){0};
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
    const SET *const set = (SET *)it->container;
    for (size_t i = it->index; i < set->capacity; i++)
    {
        const NODE *const node = set->nodes + i;
        if (node->has_data)
        {
            data = (void *) &node->data;
            if (length)
            {
                *length = sizeof(node->data);
            }
            it->index = i + 1;
            it->count++;
            break;
        }
    }

    return data;
}
