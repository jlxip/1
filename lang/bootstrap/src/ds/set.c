#include <common.h>
#include <ds/set.h>
#include <string.h>

void set_new(set *s, hash_function hash, equal_function eq, copy_function copy,
    destroy_function destroy) {
    if (s == NULL)
        return;
    if (*s != NULL)
        throw("set_new called on non-NULL set");
    *s = (set)calloc(1, sizeof(set));
    map_new(&(*s)->x, hash, eq, copy, destroy, 0, NULL, NULL, NULL, NULL);
}

#define set_assert(X)                                                          \
    if ((X) == NULL)                                                           \
        throw("called on uninitialized set");

void set_add(set s, const void *element) {
    set_assert(s);
    map_add_if_not_there(s->x, element, NULL);
}

void set_add_move(set s, void *element) {
    set_assert(s);
    map_add_movek_if_not_there(s->x, element, NULL);
}

void set_join(set s, const set other) {
    set_iterator it;
    assert(s->x->hash == other->x->hash);
    assert(s->x->copy == other->x->copy);
    assert(s->x->eq == other->x->eq);

    it = set_it_begin(other);
    while (!set_it_finished(&it)) {
        set_add(s, set_it_get(&it, void));
        set_it_next(&it);
    }
}

void set_join_move(set s, set other) {
    set_iterator it = set_it_begin(other);
    while (!set_it_finished(&it)) {
        set_add_move(s, set_it_get(&it, void));
        set_it_next(&it);
    }

    /* Shallow out for other */
    memset(other->x->buf, 0, other->x->alloc * sizeof(struct _map_t_bucket));
    set_out(&other);
}

void set_remove(set s, const void *element) {
    set_assert(s);
    map_remove(s->x, element);
}

void set_remove_if_there(set s, const void *element) {
    set_assert(s);
    map_remove_if_there(s->x, element);
}

set set_copy(const set s) {
    set ret = (set)calloc(1, sizeof(set));
    ret->x = map_copy(s->x);
    return ret;
}
void set_out(set *s) {
    set_assert(s);
    set_assert(*s);
    map_out(&((*s)->x));
    free(*s);
    *s = NULL;
}

/* --- Getters --- */
size_t set_num(set s) {
    set_assert(s);
    return map_num(s->x);
}
size_t set_empty(set s) { return set_num(s) == 0; }

size_t set_has(set s, const void *element) {
    set_assert(s);
    return map_has(s->x, element);
}

void *_set_get(set s, const void *element) {
    set_assert(s);
    return map_get_key(s->x, element, void);
}

/* --- Iterator --- */
set_iterator set_it_begin(set s) {
    set_iterator ret;
    set_assert(s);
    ret.x = map_it_begin(s->x);
    return ret;
}

void *_set_it_get(const set_iterator *it) {
    if (set_it_finished(it))
        throw("set_it_get called on finished iterator");
    return map_it_get_key(&it->x, void);
}

void set_it_next(set_iterator *it) { map_it_next(&it->x); }

size_t set_it_finished(const set_iterator *it) {
    return map_it_finished(&it->x);
}

/* --- Functors --- */
size_t hash_set(const void *ptr) {
    const set *s = (const set *)ptr;
    set_assert(s);
    set_assert(*s);
    return hash_map(&((*s)->x));
}

size_t equal_set(const void *ptra, const void *ptrb) {
    const set *a = (const set *)ptra;
    const set *b = (const set *)ptrb;
    set_assert(a);
    set_assert(*a);
    set_assert(b);
    set_assert(*b);
    return equal_map(&((*a)->x), &((*b)->x));
}

void *copy_set(const void *ptr) {
    const set *s = (const set *)ptr;
    set *ret;
    set_assert(s);
    set_assert(*s);
    ret = calloc(1, sizeof(set));
    *ret = set_copy(*s);
    return ret;
}

void destroy_set(void *a) {
    set *s = (set *)a;
    set_assert(s);
    set_assert(*s);
    set_out(s);
}
