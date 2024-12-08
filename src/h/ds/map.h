#ifndef MAP_H
#define MAP_H

#include <ds/buffer.h>

typedef struct {
    /* "Inherits" buffer if you know what I mean */
    buffer b;
    size_t ksize, vsize;
} _map_t;
typedef _map_t *map;

void map_new(map *m, size_t ksize, size_t vsize);
void map_add(map m, const void *k, const void *v);
/*void map_remove(set s, void *k);*/
void map_out(map *m);
void map_shrink(map m);

/* Getters */
size_t map_num(map m);
size_t map_empty(map m);
size_t map_has(map m, const void *k);
void *_map_get(map m, const void *k);
#define map_get(M, K, T) ((T *)_map_get(M, K))

#endif
