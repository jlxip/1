#include <common.h>
#include <ds/map.h>
#include <string.h>

/*
    Just like set, this is a terrible implementation of unordered_map
*/

void map_new(map *m, size_t ksize, size_t vsize) {
    /*
        This does absolutely zero checks for alignment, and it WILL explode
        if not taken into account when using it
    */
    if (m == NULL)
        return;
    if (*m != NULL)
        throw("map_new called on non-NULL map");
    *m = (map)malloc(sizeof(_map_t));
    (*m)->b = NULL;
    buffer_new(&(*m)->b, ksize + vsize);
    (*m)->ksize = ksize;
    (*m)->vsize = vsize;
}

#define map_assert(X)                                                          \
    if ((X) == NULL)                                                           \
        throw("called on uninitialized map");

void map_add(map m, const void *k, const void *v) {
    uint8_t *kv;
    map_assert(m);
    kv = malloc(m->ksize + m->vsize);
    memcpy(kv, k, m->ksize);
    memcpy(kv + m->ksize, v, m->vsize);
    buffer_push(m->b, kv);
    free(kv);
}

void map_out(map *m) {
    if (*m == NULL)
        return; /* Fail quietly */
    buffer_out(&(*m)->b);
    free(*m);
    *m = NULL;
}
void map_shrink(map m) {
    map_assert(m);
    buffer_shrink(m->b);
}

/* --- Getters --- */
size_t map_num(map m) {
    map_assert(m);
    return buffer_num(m->b);
}
size_t map_empty(map m) {
    map_assert(m);
    return buffer_empty(m->b);
}

static size_t map_find(map m, const void *k) {
    size_t i;
    map_assert(m);
    for (i = 0; i < buffer_num(m->b); ++i) {
        const void *x = buffer_get(m->b, i, void);
        if (0 == memcmp(x, k, m->ksize))
            return i;
    }

    return BUFFER_NOT_FOUND;
}

size_t map_has(map m, const void *k) {
    return map_find(m, k) != BUFFER_NOT_FOUND;
}

void *_map_get(map m, const void *k) {
    size_t idx = map_find(m, k);
    if (idx == BUFFER_NOT_FOUND)
        return NULL;
    return buffer_get(m->b, idx, uint8_t) + m->ksize;
}
