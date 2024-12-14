#ifndef SET_H
#define SET_H

#include <ds/map.h>

/* Value-less map: wastes a bit of memory but the code is very simple */
typedef struct {
    map x;
} _set_t; /* No direct typedef in order to not mix map_* and set_* functions */

typedef _set_t *set;

void set_new(set *s, hash_function hash, equal_function eq, copy_function copy,
    destroy_function destroy);
#define set_new_string(S)                                                      \
    set_new(S, hash_string, equal_string, copy_string, destroy_string)
#define set_new_size_t(S)                                                      \
    set_new(S, hash_size_t, equal_size_t, copy_size_t, destroy_size_t)

void set_add(set s, const void *element);
void set_join(set s, const set other);
void set_remove(set s, const void *element);
void set_remove_if_there(set s, const void *element);
set set_copy(const set s);
void set_out(set *s);
/* set_shrink? */

/* Getters */
size_t set_num(set s);
size_t set_empty(set s);
size_t set_has(set s, const void *element);

/* Iterator */
typedef struct {
    map_iterator x;
} set_iterator; /* Same as above */

set_iterator set_it_begin(set m);
void *_set_it_get(const set_iterator *it);
#define set_it_get(I, T) ((T *)_set_it_get(I))
void set_it_next(set_iterator *it);
size_t set_it_finished(const set_iterator *it);

/* Functors */
size_t hash_set(const void *s);
size_t equal_set(const void *a, const void *b);
void *copy_set(const void *a);
void destroy_set(void *a);

#endif
