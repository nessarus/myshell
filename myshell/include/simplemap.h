#pragma once
/**
 * @file    simplemap.h
 * @author  Joshua Ng
 * @brief   A hash map that assumes no collisions.
 * @date    2023-08-20
 */

struct SIMPLEMAP;

struct SIMPLEMAP*   simplemap_create    (void);
int                 simplemap_insert    (struct SIMPLEMAP* map, char *key, int value);
int                 simplemap_search    (struct SIMPLEMAP* map, char *key);
char*               simplemap_getkey    (struct SIMPLEMAP* map, int index);
int                 simplemap_getvalue  (struct SIMPLEMAP* map, int index);
int                 simplemap_delete    (struct SIMPLEMAP* map, char *key);
void                simplemap_free      (struct SIMPLEMAP* map);
