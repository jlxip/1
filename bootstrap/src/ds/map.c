#include <common.h>
#include <ds/map.h>
#include <string.h>

#define RESIZE_COEFFICIENT 0.5

typedef struct _map_t_bucket Bucket;

void map_new(map *m, hash_function hash, equal_function eq, copy_function copy,
    destroy_function destroy, size_t vsize, hash_function vhash,
    equal_function veq, copy_function vcopy, destroy_function vdestroy) {
    if (m == NULL)
        return;
    if (*m != NULL)
        throw("map_new called on non-NULL map");
    *m = (map)malloc(sizeof(_map_t));
    (*m)->buf = NULL;

    (*m)->alloc = 0;
    (*m)->used = 0;
    (*m)->erased = 0;

    (*m)->hash = hash;
    (*m)->eq = eq;
    (*m)->copy = copy;
    (*m)->destroy = destroy;

    (*m)->vsize = vsize;
    (*m)->vhash = vhash;
    (*m)->veq = veq;
    (*m)->vcopy = vcopy;
    (*m)->vdestroy = vdestroy;
}

#define map_assert(X)                                                          \
    if ((X) == NULL)                                                           \
        throw("called on uninitialized map");

static void map_extend(map m) {
    size_t i;
    size_t newalloc;
    Bucket *new;

    if (m->alloc == 0) {
        m->alloc = 2;
        m->buf = calloc(m->alloc, sizeof(Bucket));
        return;
    }

    newalloc = m->alloc * 2;
    new = calloc(newalloc, sizeof(Bucket));
    for (i = 0; i < m->alloc; ++i) {
        const Bucket *old = &m->buf[i];
        if (old->k != NULL) {
            size_t idx = old->h % newalloc;

            if (old->erased) {
                /* An erased? In the factory of non-NULLs? How queer!! */
                throw("found map inconsistency");
            }

            for (;;) {
                if (new[idx].k == NULL) {
                    new[idx] = *old;
                    break;
                }

                idx = (idx + 1) % newalloc;
            }
        }
    }

    free(m->buf);
    m->buf = new;
    m->alloc = newalloc;
    m->erased = 0;
}

static size_t map_find_slot(map m, const void *k, size_t *h) {
    size_t idx;
    map_assert(m);

    if ((m->used + m->erased) >= m->alloc * RESIZE_COEFFICIENT)
        map_extend(m);

    *h = m->hash(k);
    idx = *h % m->alloc;
    for (;;) {
        if (m->buf[idx].k == NULL) {
            if (m->buf[idx].erased == 1)
                m->erased--;
            m->used++;
            return idx; /* Found a gap */
        } else if (m->buf[idx].erased == 0) {
            /* Filled bucket, same key? */
            if (m->eq(k, m->buf[idx].k))
                return NOT_FOUND; /* 0 is taken lmao */
            /* Try the next one */
            idx = (idx + 1) % m->alloc;
        } else {
            /* An erased? In the factory of non-NULLs? How queer!! */
            throw("found map inconsistency");
        }
    }
}

static size_t _map_add(map m, const void *k, const void *v, size_t trivial) {
    size_t h, idx;
    idx = map_find_slot(m, k, &h);
    if (idx == NOT_FOUND)
        return 0;

    m->buf[idx].h = h;
    m->buf[idx].erased = 0;
    m->buf[idx].k = m->copy(k);
    if (m->vsize == 0) {
        m->buf[idx].v = NULL;
    } else if (m->vcopy == NULL || trivial) {
        /* Trivial copy */
        m->buf[idx].v = malloc(m->vsize);
        memcpy(m->buf[idx].v, v, m->vsize);
    } else {
        m->buf[idx].v = m->vcopy(v);
    }
    return 1;
}

void map_add(map m, const void *k, const void *v) {
    if (!_map_add(m, k, v, 0))
        throw("map_add with already existing key");
}

void map_add_if_not_there(map m, const void *k, const void *v) {
    _map_add(m, k, v, 0);
}

void map_add_move(map m, const void *k, void *v) {
    if (!_map_add(m, k, v, 1))
        throw("map_add_move with already existing key");
}

void map_add_move_if_not_there(map m, const void *k, void *v) {
    if (!_map_add(m, k, v, 1)) {
        /*
            Addition failed (key already exists)
            But this is a move and the programmer expects to
            not to care anymore about value. Free it now
        */
        if (m->vdestroy)
            m->vdestroy(v);
    }
}

static void _map_remove_idx(map m, size_t idx) {
    m->buf[idx].erased = 1;
    m->destroy(m->buf[idx].k);
    free(m->buf[idx].k);
    m->buf[idx].k = NULL;
    if (m->vdestroy != NULL)
        m->vdestroy(m->buf[idx].v);
    free(m->buf[idx].v); /* Trivial destruction */
    m->buf[idx].v = NULL;
    m->used--;
    m->erased++;
}

static size_t _map_remove(map m, const void *k) {
    size_t h;
    size_t idx;
    map_assert(m);

    h = m->hash(k);
    idx = h % m->alloc;
    for (;;) {
        if (m->buf[idx].k == NULL) {
            if (m->buf[idx].erased == 0) {
                /* Fail! */
                return 0;
            } else {
                /* Try the next one */
                idx = (idx + 1) % m->alloc;
            }
        } else {
            if (m->buf[idx].erased == 0) {
                /* This might be it */
                if (m->eq(k, m->buf[idx].k))
                    break; /* Got it */
                /* Try the next one */
                idx = (idx + 1) % m->alloc;
            } else {
                /* An erased? In the factory of non-NULLs? How queer!! */
                throw("found map inconsistency");
            }
        }
    }

    _map_remove_idx(m, idx);
    return 1;
}

void map_remove(map m, const void *k) {
    if (!_map_remove(m, k))
        throw("map_remove with non-existing key");
}
void map_remove_if_there(map m, const void *k) { _map_remove(m, k); }

map map_copy(map m) {
    map ret = NULL;
    map_iterator it;

    map_new(&ret, m->hash, m->eq, m->copy, m->destroy, m->vsize, m->vhash,
        m->veq, m->vcopy, m->vdestroy);
    it = map_it_begin(m);
    while (!map_it_finished(&it)) {
        const void *k = map_it_get_key(&it, void);
        const void *v = map_it_get_value(&it, void);
        map_add(ret, k, v);
        map_it_next(&it);
    }

    return ret;
}

void map_out(map *m) {
    map_iterator it;
    if (*m == NULL)
        return; /* Fail quietly */

    it = map_it_begin(*m);
    while (!map_it_finished(&it)) {
        _map_remove_idx(*m, it.x - (*m)->buf);
        map_it_next(&it);
    }

    free((*m)->buf);
    free(*m);
    *m = NULL;
}

/* --- Getters --- */
size_t map_num(map m) {
    map_assert(m);
    return m->used;
}
size_t map_empty(map m) {
    map_assert(m);
    return m->used == 0;
}

static size_t map_find(map m, const void *k) {
    size_t h;
    size_t idx;
    map_assert(m);
    if (m->alloc == 0)
        return NOT_FOUND;

    h = m->hash(k);
    idx = h % m->alloc;
    for (;;) {
        if (m->buf[idx].k == NULL) {
            if (m->buf[idx].erased == 0) {
                /* Tough luck */
                return NOT_FOUND;
            } else {
                /* Try the next one */
                idx = (idx + 1) % m->alloc;
            }
        } else {
            if (m->buf[idx].erased == 0) {
                /* This might be it */
                if (m->eq(k, m->buf[idx].k))
                    return idx; /* Found it */
                /* Try the next one */
                idx = (idx + 1) % m->alloc;
            } else {
                /* An erased? In the factory of non-NULLs? How queer!! */
                throw("found map inconsistency");
            }
        }
    }
}

size_t map_has(map m, const void *k) { return map_find(m, k) != NOT_FOUND; }

void *_map_get(map m, const void *k) {
    size_t idx = map_find(m, k);
    if (idx == NOT_FOUND)
        return NULL;
    return m->buf[idx].v;
}

/* --- Iterator --- */
map_iterator map_it_begin(map m) {
    map_iterator ret;
    map_assert(m);

    ret.m = m;
    ret.x = m->buf;
    if (m->buf == NULL)
        return ret;

    if (m->buf[0].k == NULL || m->buf[0].erased)
        map_it_next(&ret);
    return ret;
}

const void *_map_it_get_key(const map_iterator *it) {
    if (map_it_finished(it))
        throw("map_it_get_key called on finished iterator");
    return it->x->k;
}

void *_map_it_get_value(const map_iterator *it) {
    if (map_it_finished(it))
        throw("map_it_get_value called on finished iterator");
    return it->x->v;
}

void map_it_next(map_iterator *it) {
    const Bucket *end = &it->m->buf[it->m->alloc];
    do {
        it->x++;
        if (it->x == end) {
            it->x = NULL;
            return;
        }
    } while (it->x->k == NULL || it->x->erased);
}

size_t map_it_finished(const map_iterator *it) { return it->x == NULL; }

/* --- Functors --- */
size_t hash_map(const void *ptr) {
    const map *m = (const map *)ptr;
    size_t ret = 0;
    map_iterator it;

    map_assert(m);
    map_assert(*m);
    if ((*m)->vhash == NULL && (*m)->vsize)
        throw("hash_map called on map without vhash");

    it = map_it_begin(*m);
    while (!map_it_finished(&it)) {
        size_t hashk, hashv, hashkv;
        hashk = it.x->h;
        if ((*m)->vhash == NULL)
            hashv = 0;
        else
            hashv = (*m)->vhash(map_it_get_value(&it, void));
        hashkv = combine_hashes(hashk, hashv);
        /* XOR, because elements are unordered */
        ret ^= hashkv;
        map_it_next(&it);
    }
    ret = hash_size_t(&ret);
    combine_hashes(ret, hash_size_t(&((*m)->used))); /* Just in case really */
    combine_hashes(ret, hash_size_t(&((*m)->vsize)));
    return ret;
}

#define CHECK_EQ(F)                                                            \
    do {                                                                       \
        if ((*a)->F != (*b)->F)                                                \
            return 0;                                                          \
    } while (0)
size_t equal_map(const void *ptra, const void *ptrb) {
    map_iterator it;
    const map *a = (const map *)ptra;
    const map *b = (const map *)ptrb;
    map_assert(a);
    map_assert(*a);
    map_assert(b);
    map_assert(*b);

    if ((*a)->veq == NULL && (*a)->vsize)
        throw("equal_map called on map without veq");

    CHECK_EQ(used);
    CHECK_EQ(hash);
    CHECK_EQ(eq);
    CHECK_EQ(copy);
    CHECK_EQ(destroy);
    CHECK_EQ(vsize);
    CHECK_EQ(veq);
    /* Don't check the rest of v-functors since they're optional */

    /*
        Since both have the same number of elements,
        and elements can only appear once,
        just check that all elements of A are in B
    */
    it = map_it_begin(*a);
    while (!map_it_finished(&it)) {
        void *key, *val;
        size_t bidx;

        /* Check key is there */
        key = map_it_get_key(&it, void);
        bidx = map_find(*b, key);
        if (bidx == NOT_FOUND)
            return 0;

        /* Check value if appropiate */
        if ((*a)->vsize) {
            val = (*b)->buf[bidx].v;
            if (!(*a)->veq(map_it_get_value(&it, void), val))
                return 0;
        }

        /* That's good */
        map_it_next(&it);
    }

    return 1;
}

void *copy_map(const void *ptr) {
    const map *m = (const map *)ptr;
    map *ret;
    map_assert(m);
    map_assert(*m);
    ret = calloc(1, sizeof(map));
    *ret = map_copy(*m);
    return ret;
}

void destroy_map(void *ptr) {
    map *m = (map *)ptr;
    map_assert(m);
    map_assert(*m);
    map_out(m);
}
