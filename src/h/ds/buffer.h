#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>

#define BUFFER_NOT_FOUND (~0ul)
/* Thanks Applus for this */
#define BUFFER_SIGNATURE 0xFABADAC0C1D0

typedef struct {
    volatile size_t signature;
    size_t datasize;
    void *a;
    size_t alloc;
    size_t used;
} _buffer_t;

typedef _buffer_t *buffer;

void buffer_new(buffer *buf, size_t datasize);
void buffer_push(buffer buf, const void *element);
void buffer_pop(buffer buf);
void buffer_remove(buffer buf, size_t idx);
void buffer_out(buffer *buf);
void buffer_shrink(buffer buf);
void buffer_sort(buffer buf, int (*comp)(const void *, const void *));

#define buffer_iter(X, T, x)                                                   \
    x = (X)->a ? (T *)((X)->a) : NULL;                                         \
    (x - (T *)((X)->a)) < (long int)((X)->used);                               \
    ++x

/* Getters */
size_t buffer_num(buffer buf);
size_t buffer_empty(buffer buf);
size_t buffer_find(buffer buf, const void *element);
void *_buffer_get(buffer buf, size_t idx);
#define buffer_get(B, I, T) ((T *)_buffer_get(B, I))
#define buffer_front(B, T) buffer_get(B, 0, T)
#define buffer_back(B, T) buffer_get(B, buffer_num(B) - 1, T)

#endif