#ifndef SET_H
#define SET_H

/*
    A really terrible implementation of unordered_set
    It is not worth to put much effort here since this is the stage0 compiler
*/

#include <ds/buffer.h>

typedef buffer set;

void set_new(set *s, size_t datasize);
void set_add(set s, void *element);
void set_remove(set s, void *element);
void set_out(set *s);
void set_shrink(set s);

#define set_iter(X, T, x) buffer_iter(X, T, x)

/* Getters */
size_t set_num(set s);
size_t set_empty(set s);
size_t set_has(set s, void *element);

#endif
