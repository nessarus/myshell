/**
 * @file    simplemap.c
 * @author  Joshua Ng.
 * @brief   A hash map that assumes no collisions.
 * @date    2023-08-19
 */

#include "simplemap.h"
#include "myshell.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief simplemap uses a fixed lookup size.
 */
#define ARRAY_SIZE 7

/**
 * @brief The data structure to hold key and value.
 */
typedef struct 
{
    char    *key; 
    int     value;
} Pair;

/**
 * @brief The simplemap data structure.
 */
typedef struct SIMPLEMAP
{
    Pair array[ARRAY_SIZE];
} SIMPLEMAP;

/**
 * @brief A simple string hash function. Sums the string characters.
 * 
 * @param key   The string to hash.
 * @return The hashed result.  
 */
int hash(char *key) 
{
    int sum = 0;

    for (char* ch = key; *ch != '\0'; ch++) 
    {
        sum += *ch;
    }

    return sum % ARRAY_SIZE;
}

/**
 * @brief Constructor for a simplemap struct.
 * 
 * @return A memory allocated simplemap struct pointer.
 */
SIMPLEMAP* simplemap_create(void)
{
    SIMPLEMAP* map = (SIMPLEMAP*) calloc(1, sizeof(SIMPLEMAP));
    check_allocation(map);
    return map;
}

/**
 * @brief Inserts an entry to the simplemap.
 * 
 * @param map   A pointer to the simplemap.
 * @param key   The string key.
 * @param value The value.
 * @return The index of the entry. 
 */
int simplemap_insert(SIMPLEMAP *map, char *key, int value)
{
    int index = hash(key);

    if (map->array[index].key != NULL)
    {
        fprintf(stderr, "%s: %s %s keys collision in simplemap_insert()\n", 
            name0, key, map->array[index].key);
        exit(EXIT_FAILURE);
    }

    char* key_copy = strdup(key);
    check_allocation(key_copy);
    map->array[index] = (Pair) {key_copy, value};
    return index;
}

/**
 * @brief Searches for an entry.
 * 
 * @param map   The map to search.
 * @param key   The key of the entry.
 * @return The index of the entry.
 */
int simplemap_search(SIMPLEMAP *map, char *key)
{
    int index = hash(key);
    char *entry = map->array[index].key;
    
    if ((entry != NULL) && (strcmp(entry, key) == 0)) 
    {
        return index;
    } 

    return -1;
}

/**
 * @brief Gets the key of an entry.
 * 
 * @param map   The map to get from.
 * @param index The index of the entry.
 * @return The key of the entry.
 */
char *simplemap_getkey(SIMPLEMAP *map, int index)
{
    return map->array[index].key;
}

/**
 * @brief Gets the value of an entry.
 * 
 * @param map   The map to get from.
 * @param index The index of the entry.
 * @return The value of the entry.
 */
int simplemap_getvalue(SIMPLEMAP *map, int index)
{
    return map->array[index].value;
}

/**
 * @brief Deletes an entry.
 * 
 * @param map   The map to search.
 * @param key   The key of the entry.
 * @return The index of the entry.
 */
int simplemap_delete(SIMPLEMAP *map, char *key)
{
    int index = simplemap_search(map, key);
    
    if (index != -1)
    {
        free(map->array[index].key);
        map->array[index].key = NULL;
        map->array[index].value = 0;
    }

    return index;
}

/**
 * @brief Frees the map.
 * 
 * @param map The map to free.
 */
void simplemap_free(SIMPLEMAP *map)
{
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        free(map->array[i].key);
    }

    free(map);
    map = NULL;
}
