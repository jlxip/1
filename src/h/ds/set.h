#ifndef SET_H
#define SET_H

#include <ds/buffer.h>

typedef buffer set;

void set_new(set *s, size_t datasize);
void set_add(set s, const void *element);
void set_remove(set s, const void *element);
void set_out(set *s);
void set_shrink(set s);

#define set_iter(X, T, x) buffer_iter(X, T, x)

/* Getters */
size_t set_num(set s);
size_t set_empty(set s);
size_t set_has(set s, const void *element);

#endif
