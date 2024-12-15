#ifndef FUNCTORS_H
#define FUNCTORS_H

#include <stddef.h>

size_t hash_buffer(const void *ptr, size_t sz);
size_t combine_hashes(size_t a, size_t b);

typedef size_t (*hash_function)(const void *ptr);
typedef size_t (*equal_function)(const void *a, const void *b);
typedef void *(*copy_function)(const void *a);
typedef void (*destroy_function)(void *a);

/* --- Commons --- */

size_t hash_string(const void *s);
size_t equal_string(const void *a, const void *b);
void *copy_string(const void *a);
void destroy_string(void *a);

size_t hash_size_t(const void *x);
size_t equal_size_t(const void *a, const void *b);
void *copy_size_t(const void *a);
void destroy_size_t(void *a);

#endif
