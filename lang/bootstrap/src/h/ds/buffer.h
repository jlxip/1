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
void buffer_push(buffer buf, const void *element);
void buffer_push_front(buffer buf, const void *element);
void buffer_set(buffer buf, size_t idx, const void *element);
void buffer_pop(buffer buf);
void buffer_remove(buffer buf, size_t idx);
buffer buffer_copy(const buffer buf);
void buffer_out(buffer *buf);
void buffer_shrink(buffer buf);
void buffer_sort(buffer buf, int (*comp)(const void *, const void *));
void buffer_reverse(buffer buf);

/* Getters */
size_t buffer_num(buffer buf);
size_t buffer_empty(buffer buf);
size_t buffer_find(buffer buf, const void *element);
void *_buffer_get(buffer buf, size_t idx);
#define buffer_get(B, I, T) ((T *)_buffer_get(B, I))
#define buffer_front(B, T) buffer_get(B, 0, T)
#define buffer_back(B, T) buffer_get(B, buffer_num(B) - 1, T)
#define buffer_get_raw(B, T) buffer_get(B, 0, T)

#endif
