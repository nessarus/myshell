#pragma once
/**
 * @file hashset.h
 * @author Joshua Ng.
 * @brief Implements a set data container.
 * @date 2023-09-07
 */

#include "iterator.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Interface for a set to work with a custom container element.
 *
 * Container writers implement this interface to allow the generic SET
 * to access keys, compute hashes, and compare keys stored in elements.
 */
typedef struct {
    /**
     * @brief Compute the hash of an element's key.
     *
     * Container writers provide this function. The hash value
     * must be consistent with equals(): if two elements are
     * equal, their hashes must also be equal.
     *
     * @param element Pointer to the element.
     * @return Hash value of the element's key.
     */
    size_t (*hash)(const void *element);

    /**
     * @brief Check if two elements are equal.
     * @param element1 Pointer to the first element.
     * @param element2 Pointer to the second element.
     * @return true if keys are equal, false otherwise.
     */
    bool (*equals)(const void *element1, const void *element2);
} HASHSET_INTERFACE;

/**
 * @brief Generic hash set structure.
 *
 * Stores elements in an array of void pointers and uses the HASHSET_INTERFACE
 * to access keys and compute hashes.
 */
typedef struct {
    /** Interface for container-specific element access. */
    HASHSET_INTERFACE interface;

    /** Array of pointers to elements. */
    void **elements;

    /** Current number of elements in the set. */
    size_t size;

    /** Total number of slots allocated in the array. */
    size_t capacity;
} HASHSET;

void **hashset_resize(HASHSET *set, void **elements, size_t capacity);
bool hashset_contains(const HASHSET *set, const void *element);

/**
 * @brief Result type for insertion and removal operations.
 */
typedef struct {
    void *element; /**< The element involved in the operation */
    bool success;  /**< true if insertion/removal succeeded */
} HASHSET_RESULT;

HASHSET_RESULT hashset_insert(HASHSET *set, void *element);
HASHSET_RESULT hashset_remove(HASHSET *set, const void *element);
ITERATOR hashset_iterator(const HASHSET *set);
bool hashset_iterator_has_next(const ITERATOR *it);
void *hashset_iterator_next(ITERATOR *it);
