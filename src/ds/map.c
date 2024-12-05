#include <common.h>
#include <ds/map.h>
#include <string.h>

/*
    Just like set, this is a terrible implementation of unordered_map
*/

typedef union {
    struct {
        void *k, *v;
    } data;
    struct {
        size_t k, v;
    } sizes;
} KV;

void map_new(map *m, size_t ksize, size_t vsize) {
    KV sizes;
    sizes.sizes.k = ksize;
    sizes.sizes.v = vsize;
    buffer_new(m, sizeof(KV));
    buffer_push(*m, &sizes);
}

static size_t map_getksize(map m) { return ((const KV *)m->a)->sizes.k; }
static size_t map_getvsize(map m) { return ((const KV *)m->a)->sizes.v; }

void map_add(map m, const void *k, const void *v) {
    KV entry;
    size_t ksize = map_getksize(m);
    size_t vsize = map_getvsize(m);

    entry.data.k = malloc(ksize);
    entry.data.v = malloc(vsize);
    memcpy(entry.data.k, k, ksize);
    memcpy(entry.data.v, v, vsize);

    buffer_push(m, &entry);
}

void map_out(map *m) { buffer_out(m); }
void map_shrink(map m) { buffer_shrink(m); }

/* --- Getters --- */
size_t map_num(map m) { return buffer_num(m); }
size_t map_empty(map m) { return buffer_empty(m); }

static size_t map_find(map m, const void *k) {
    const KV *sizes;
    const KV *x;
    size_t idx = 0;

    assert(k);
    for (buffer_iter(m, KV, x)) {
        if (!idx) {
            sizes = x;
            ++idx;
            continue;
        }

        if (0 == memcmp(x->data.k, k, sizes->sizes.k))
            return idx;
        ++idx;
    }

    return BUFFER_NOT_FOUND;
}

size_t map_has(map m, const void *k) {
    return map_find(m, k) != BUFFER_NOT_FOUND;
}

void *_map_get(map m, const void *k) {
    size_t idx = map_find(m, k);
    KV *kv;
    if (idx == BUFFER_NOT_FOUND)
        return NULL;
    kv = buffer_get(m, idx, KV);
    return kv->data.v;
}
