/**
 * @file    hashset.h
 * @author  Joshua Ng.
 * @brief   Implements a hash set data container.
 * @date    2023-09-07
 */

#include "hashset.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief A helper function to insert an element into the set assuming no duplicates.
 *
 * @param set   A pointer to a set.
 * @param element  The element to insert.
 * @return True if element was inserted and false otherwise.
 */
static bool hashset_insert_element(HASHSET *set, void *element)
{
    const size_t i = set->interface.hash(element) % set->capacity;

    for (size_t j = 0; j < set->capacity; j++)
    {
        const size_t index = ((i + j) % set->capacity);

        if (set->elements[index] == NULL)
        {
            set->elements[index] = element;
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Replace the internal element array with a new one.
 *
 * The user controls when to resize. This function does not allocate
 * or free memory itself. It only rehashes existing elements into the
 * new array.
 *
 * @param set Pointer to the HASHSET structure.
 * @param elements Pointer to the new element array (must be preallocated).
 * @param capacity Size of the new element array.
 * @return Pointer to the old element array (caller is responsible for
 * freeing or reusing it) or Null if hashset_resize failed.
 */
void **hashset_resize(HASHSET *set, void **elements, size_t capacity)
{
    if (set->size > capacity)
    {
        return NULL;
    }
    
    HASHSET temp = *set;
    temp.capacity = capacity;
    temp.elements = elements;

    for (size_t i = 0; i < set->capacity; i++)
    {
        void *const element = set->elements[i];

        if (element != NULL)
        {
            if (!hashset_insert_element(&temp, element))
            {
                return NULL;
            }
        }
    }
    
    void **saved = set->elements;
    *set = temp;
    
    return saved;
}

/**
 * @brief Check whether a given element is present in the set.
 *
 * This function does not allocate, free, or modify any elements.
 *
 * @param set Pointer to the HASHSET.
 * @param element Pointer to the element to check.
 * @return true if the element is contained in the set, false otherwise.
 */
bool hashset_contains(const HASHSET *set, const void *element)
{
    const size_t i = set->interface.hash(element) % set->capacity;

    for (int j = 0; j < set->capacity; j++)
    {
        const size_t index = (i + j) % set->capacity;
        const void *const hashset_element = set->elements[index];

        if (hashset_element == NULL)
        {
            break;
        }

        if (set->interface.equals(hashset_element, element))
        {
            return true;
        }
    }

    return false;
}

/**
 * @brief Insert an element into the set.
 *
 * The set does not copy or allocate memory for the element; it only
 * stores the pointer. The caller is responsible for allocating and
 * freeing the element's memory.
 *
 * @param set Pointer to the HASHSET.
 * @param element Pointer to the element to insert.
 * @return HASHSET_RESULT containing:
 *   - .element = the element inserted, or an existing element with
 *                the same key if already present.
 *   - .success = true if insertion succeeded, false if element already existed.
 */
HASHSET_RESULT hashset_insert(HASHSET *set, void *element)
{
    HASHSET_RESULT result = {0};
    const size_t i = set->interface.hash(element) % set->capacity;

    for (size_t j = 0; j < set->capacity; j++)
    {
        const size_t index = (i + j) % set->capacity;
        void *const hashset_element = set->elements[index];

        if (hashset_element == NULL)
        {
            set->elements[index] = element;
            set->size++;
            result = (HASHSET_RESULT){.element = element, .success = true};
            break;
        }

        if (set->interface.equals(hashset_element, element))
        {
            result.element = hashset_element;
            break;
        }
    }

    return result;
}

/**
 * @brief Remove an element from the set.
 *
 * The set only removes the pointer and does not free memory.
 * The caller is responsible for freeing or reusing the element.
 *
 * @param set Pointer to the HASHSET.
 * @param element Pointer to the element to remove.
 * @return HASHSET_RESULT containing:
 *   - .element = the removed element, or NULL if not found.
 *   - .success = true if removal succeeded, false otherwise.
 */
static void hashset_cleanup(const HASHSET *set, size_t index)
{
    void **empty = set->elements + index;
    size_t iempty = index;

    for (size_t j = 1; j < set->capacity; j++)
    {
        const size_t i = ((index + j) % set->capacity);
        void **const element = set->elements + i;

        if (*element == NULL)
        {
            break;
        }

        const size_t hash = set->interface.hash(*element);
        const size_t ihash = hash % set->capacity;
        if ((ihash <= iempty) && ((i > iempty) || (ihash > i)))
        {
            *empty = *element;
            empty = element;
            *empty = NULL;
            iempty = i;
        }
    }
}

/**
 * @brief Remove an element from the set.
 *
 * @param set Pointer to the HASHSET.
 * @param element Pointer to the element to remove.
 * @return HASHSET_RESULT containing the removed element and success flag.
 */
HASHSET_RESULT hashset_remove(HASHSET *set, const void *element)
{
    HASHSET_RESULT result = {0};
    const size_t i = set->interface.hash(element) % set->capacity;
    size_t index = i;

    for (size_t j = 0; j < set->capacity; j++)
    {
        index = (i + j) % set->capacity;
        void *const hashset_element = set->elements[index];

        if (hashset_element == NULL)
        {
            break;
        }
        
        if (set->interface.equals(hashset_element, element))
        {
            result = (HASHSET_RESULT){.element = hashset_element, .success = true};
            set->elements[index] = NULL;
            set->size--;
            break;
        }
    }

    if (result.success)
    {
        hashset_cleanup(set, index);
    }

    return result;
}

/**
 * @brief Create an iterator over the set.
 *
 * @param set Pointer to the HASHSET to iterate.
 * @return An ITERATOR that can be used with has_next / next.
 */
ITERATOR hashset_iterator(const HASHSET *set)
{
    return (ITERATOR){
        .state = (void *)set,
        .position.index = 0,
        .has_next = &hashset_iterator_has_next,
        .next = &hashset_iterator_next};
}

/**
 * @brief Check whether the iterator has more elements.
 *
 * Non-mutating: this function does not advance the iterator.
 * 
 * @param it Pointer to the iterator.
 * @return true if more elements are available, false otherwise.
 */
bool hashset_iterator_has_next(const ITERATOR* it)
{
    const HASHSET *const set = (const HASHSET *)it->state;
    for (size_t i = it->position.index; i < set->capacity; i++)
    {
        if (set->elements[i] != NULL)
        {
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Advance the iterator and return the current element.
 *
 * @param it Pointer to the iterator.
 * @return Pointer to the current element, or NULL if iteration is finished.
 */
void* hashset_iterator_next(ITERATOR* it)
{
    void *element = NULL;
    const HASHSET *const set = (const HASHSET *)it->state;
    
    for (size_t i = it->position.index; i < set->capacity; i++)
    {
        it->position.index++;
        if (set->elements[i] != NULL)
        {
            element = set->elements[i];
            break;
        }
    }

    return element;
}
