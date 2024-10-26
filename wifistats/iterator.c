/**
 * @file    iterator.c
 * @author  Joshua Ng.
 * @brief   Implements an iterator.
 * @date    2024-08-25
 */

#include "iterator.h"

/**
 * @brief Checks if there are any further elements
 *
 * @param it        A pointer to the iterator.
 * @return  Returns true if the iteration has more elements.
 */
bool iterator_has_next(ITERATOR *it)
{
    return it->has_next(it);
}

/**
 * @brief Returns a pointer to the next element.
 *
 * @param it        A pointer to the iterator.
 * @param length    Outputs the length of the element.
 * @return  Returns a pointer to the next element.
 */
void* iterator_next(ITERATOR *it)
{
    return it->next(it);
}
