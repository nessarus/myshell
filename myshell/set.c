/**
 * @file    set.h
 * @author  Joshua Ng.
 * @brief   Implements a set data container.
 * @date    2023-09-07
 */

#include "set.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Number of buckets in the hash table.
 */
#define BUCKETS 10

/**
 * @brief A node structure for the linked list.
 */
typedef struct NODE 
{
    int data;           // the element stored in the node
    struct NODE *next;  // pointer to the next node
} NODE;

/**
 * @brief A set structure for the hash table.
 */
typedef struct SET 
{
    NODE *table[BUCKETS];   // An array of head nodes of the linked lists
    int size;               // The amount of items in the set.
} SET;

/**
 * @brief A hash function that maps an element to an index in the range 
 * [0, BUCKETS - 1]
 * 
 * @param value The value to be hashed.
 * @return The computed the hash value using a simple modulo operation.
 */
static int hash(int value)
{
    return abs(value) % BUCKETS;
}

/**
 * @brief A function that creates a new node with the given element and 
 * returns its pointer.
 * 
 * @param value     The value of the node.
 * @return A memory allocated node.
 */
static NODE *node_create(int value) 
{
    NODE *node = calloc(1, sizeof(NODE));
    check_allocation(node);
    node->data = value;
    return node;
}

/**
 * @brief A function that creates a new set and returns its pointer.
 * 
 * @return A memory allocated set.
 */
SET *set_create(void) 
{
    SET *set = calloc(1, sizeof(SET));
    check_allocation(set);
    return set;
}

/**
 * @brief Set the size of the set.
 * 
 * @param set   The set data structure.
 * @return The amount of items in the set.
 */
int set_size(SET *set)
{
    return set->size;
}

/**
 * @brief A function that checks if an element is in the set.
 * 
 * @param set   The set data structure.
 * @param value The value to check.
 * @return True if element is in the set and false otherwise.
 */
bool set_contains(SET *set, int value)
{
    if (set->size == 0)
    {
        return false;
    }

    int h = hash(value);

    for (NODE *node = set->table[h]; node != NULL; node = node->next)
    {
        if (node->data == value) 
        {
            return true;
        }
    }

    return false;
}

/**
 * @brief A function that inserts an element into the set if it is not 
 * already present and returns if successful or not. 
 * 
 * @param set   The set data structure.
 * @param value The value to insert.
 * @return True if the value was inserted successfully and false otherwise.
 */
bool set_insert(SET *set, int value) 
{
    if (set_contains(set, value)) 
    {
        return false;
    }

    int index = hash(value);
    NODE *node = node_create(value);
    node->next = set->table[index]; 
    set->table[index] = node;
    set->size++;
    return true;
}

/**
 * @brief A function that removes an element from the set if it is present 
 * and returns if successful or not.
 * 
 * @param set   The set data structure.
 * @param value The value to remove.
 * @return True if the value was successfully removed and false otherwise. 
 */
bool set_remove(SET *set, int value) 
{
    int h = hash(value);
    NODE** node = &set->table[h];

    while (*node != NULL) 
    {
        if ((*node)->data == value) 
        {
            NODE *next = (*node)->next;
            free(*node);
            *node = next;
            set->size--;
            return true;
        }

        node = &(*node)->next;
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
    for (int i = 0; i < BUCKETS; i++) 
    {
        NODE *node = set->table[i];

        while (node != NULL) 
        {
            NODE *temp = node;
            node = node->next;
            free(temp);
        }
    }

    free(set);
}

/**
 * @brief Define an iterator structure for the hashmap
 */
typedef struct SET_ITERATOR 
{
    SET*        set;        // The pointer to the hashmap
    int         index;      // The current bucket index
    NODE*       current;    // The current node pointer
} SET_ITERATOR;

/**
 * @brief Initialize an iterator for the hashmap.
 * 
 * @param set   The pointer to the hashmap    
 * @return A pointer to a memory allocated hashmap iterator.
 */
SET_ITERATOR* set_iterator_create(SET* set) 
{
    SET_ITERATOR* it = malloc(sizeof(SET_ITERATOR));
    check_allocation(it);
    *it = (SET_ITERATOR){.set = set, .index = 0, .current = set->table[0]};

    if (it->current == NULL)
    {
        it->index++;

        while (it->index < BUCKETS)
        {
            if (it->set->table[it->index] != NULL)
            {
                it->current = it->set->table[it->index];
                break;
            }

            it->index++;
        }
    }

    return it;
}

/**
 * @brief Advance the iterator to the next element in the hashmap.
 * 
 * @param it    The iterator.
 */
void set_iterator_next(SET_ITERATOR* it) 
{
    if (it->current != NULL)
    {
        it->current = it->current->next;
    }

    if (it->current == NULL)
    {
        it->index++;

        while ((it->index < BUCKETS) && (it->set->table[it->index] == NULL))
        {
            it->index++;
        }

        if (it->index < BUCKETS)
        {
            it->current = it->set->table[it->index];
        }
    }
}

/**
 * @brief Check if the iterator has more elements to iterate over.
 * 
 * @param it    The iterator.
 * @return Return true if the current iterator node is valid.
 */
bool set_iterator_has_next(SET_ITERATOR* it)
{
    return ((it->index < BUCKETS) && (it->current != NULL));
}

/**
 * @brief Get the value of the set element at the current iterator.
 * 
 * @param it    The iterator.
 * @return The value of the element.
 */
int set_iterator_get(SET_ITERATOR* it) 
{
    return it->current->data;
}

/**
 * @brief Free the memory allocated for the iterator.
 * 
 * @param it    The iterator.
 */
void set_iterator_free(SET_ITERATOR* it) 
{
    free(it);
}
