#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>

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
void _buffer_push(buffer buf, void *element);
void buffer_out(buffer *buf);
void buffer_shrink(buffer buf);
void buffer_sort(buffer buf, int (*comp)(const void *, const void *));

#define buffer_push(B, E) _buffer_push(B, (void *)E)

#define buffer_iter(X, T, x)                                                   \
    x = X->a ? (T *)X->a : NULL;                                               \
    (x - (T *)X->a) < (long int)X->used;                                       \
    ++x

#endif
