#pragma once
/**
 * @file    iterator.h
 * @author  Joshua Ng.
 * @brief   Implements an iterator.
 * @date    2024-08-25
 */

#include <stddef.h>
#include <stdbool.h>

/**
 * @struct ITERATOR
 * @brief Generic iterator interface for containers.
 *
 * This struct defines an interface for iterating over any container.
 * Container writers implement iteration by providing a state struct and
 * functions to advance and check the iterator.
 */
typedef struct ITERATOR
{
    /**
     * @brief Container-specific iterator state.
     *
     * - For arrays: store length or other metadata needed for iteration.
     * - For linked lists: store head pointer, bucket, or traversal info.
     */
    void *state;

    /**
     * @brief Current position in the container.
     *
     * Use `ptr` for pointer-based containers (linked lists, trees) and
     * `index` for index-based containers (arrays, vectors).
     */
    union
    {
        void **ptr; /**< Pointer to current element (pointer-based containers) */
        size_t index; /**< Current index (index-based containers) */
    } position;

    /**
     * @brief Check if there are more elements in the container.
     * @param it Pointer to the iterator
     * @return true if more elements are available, false otherwise.
     *
     * Notes for implementers:
     * - Should not advance the iterator.
     * - For arrays: compare `position.index` with array length.
     * - For pointer-based containers: check if `position.ptr` is not NULL.
     */
    bool (*has_next)(const struct ITERATOR *it);

    /**
     * @brief Advance the iterator and return the current element.
     * @param it Pointer to the iterator
     * @return Pointer to current element, or NULL if iteration finished.
     *
     * Notes for implementers:
     * - Return the element currently pointed to by the iterator.
     * - Advance `position` to the next element.
     * - For arrays: increment `position.index`.
     * - For pointer-based containers: move `position.ptr` to next element.
     */
    void* (*next)(struct ITERATOR *it);

} ITERATOR;
