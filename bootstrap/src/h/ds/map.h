#ifndef MAP_H
#define MAP_H

#include <ds/buffer.h>
#include <ds/functors.h>

/*
    An uncomplicated unordered_map implementation
    Values are always shallow-copied
*/

struct _map_t_bucket {
    size_t h;
    size_t erased;
    void *k, *v;
};

typedef struct {
    /* buffer */
    struct _map_t_bucket *buf;
    /* state */
    size_t alloc;
    size_t used;
    size_t erased;
    /* key properties */
    hash_function hash;
    equal_function eq;
    copy_function copy;
    destroy_function destroy;
    /* value properties */
    size_t vsize;              /* 0 if no value (set) */
    hash_function vhash;       /* Only for: hash_map */
    equal_function veq;        /* Only for: equal_map */
    copy_function vcopy;       /* Only for: copy_map or non-trivial copy */
    destroy_function vdestroy; /* Only for: non-trivial destruction */
} _map_t;

typedef _map_t *map;

void map_new(map *m, hash_function hash, equal_function eq, copy_function copy,
    destroy_function destroy, size_t vsize, hash_function vhash,
    equal_function veq, copy_function vcopy, destroy_function vdestroy);
#define map_new_string(M, VS, VH, VE, VC, VD)                                  \
    map_new(M, hash_string, equal_string, copy_string, destroy_string, VS, VH, \
        VE, VC, VD)
#define map_new_size_t(M, VS, VH, VE, VC, VD)                                  \
    map_new(M, hash_size_t, equal_size_t, copy_size_t, destroy_size_t, VS, VH, \
        VE, VC, VD)

void map_add(map m, const void *k, const void *v);
void map_add_if_not_there(map m, const void *k, const void *v);
void map_remove(map m, const void *k);
void map_remove_if_there(map m, const void *k);
map map_copy(const map m);
void map_out(map *m);
/* map_shrink? */

/* Getters */
size_t map_num(map m);
size_t map_empty(map m);
size_t map_has(map m, const void *k);
void *_map_get(map m, const void *k);
#define map_get(M, K, T) ((T *)_map_get(M, K))

/* Iterator */
typedef struct {
    map m;
    struct _map_t_bucket *x;
} map_iterator;

map_iterator map_it_begin(map m);
const void *_map_it_get_key(const map_iterator *it);
#define map_it_get_key(I, T) ((T *)_map_it_get_key(I))
void *_map_it_get_value(const map_iterator *it);
#define map_it_get_value(I, T) ((T *)_map_it_get_value(I))
void map_it_next(map_iterator *it);
size_t map_it_finished(const map_iterator *it);

/* Functors */
size_t hash_map(const void *m);
size_t equal_map(const void *a, const void *b);
void *copy_map(const void *a);
void destroy_map(void *a);

#endif
