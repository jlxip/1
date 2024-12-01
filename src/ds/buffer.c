#include <common.h>
#include <ds/buffer.h>
#include <stdlib.h>
#include <string.h>

static void buffer_reset(buffer buf) {
    buf->a = NULL;
    buf->alloc = buf->used = 0;
}

void buffer_new(buffer *buf, size_t datasize) {
    if (buf == NULL)
        return;
    if (*buf != NULL)
        throw("buffer_new called on non-NULL buffer");
    *buf = (buffer)malloc(sizeof(_buffer_t));
    (*buf)->signature = BUFFER_SIGNATURE;
    (*buf)->datasize = datasize;
    buffer_reset(*buf);
}

static void buffer_2x(buffer buf) {
    buf->alloc = buf->alloc ? 2 * buf->alloc : 1;
    buf->a = realloc(buf->a, buf->alloc * buf->datasize);
}

#define buffer_assert(X)                                                       \
    if (X == NULL || X->signature != BUFFER_SIGNATURE)                         \
        throw("buffer_push called on uninitialized buffer");

void buffer_push(buffer buf, void *element) {
    char *target = NULL;
    buffer_assert(buf);
    if (buf->used == buf->alloc)
        buffer_2x(buf);
    target = (char *)buf->a + buf->used++ * buf->datasize;
    memcpy(target, element, buf->datasize);
}

void buffer_pop(buffer buf) {
    buffer_assert(buf);
    if (buf->used == 0)
        throw("buffer_pop called on empty buffer");
    buf->used--;
}

void buffer_remove(buffer buf, size_t idx) {
    char *data = (char *)buf->a;
    size_t src, dst;

    buffer_assert(buf);
    if (idx >= buf->used)
        throw("invalid idx in call to buffer_remove");

    dst = idx * buf->datasize;
    src = dst + buf->datasize;
    memmove(data + dst, data + src, buf->datasize);
    buf->used--;
}

void buffer_shrink(buffer buf) {
    buffer_assert(buf);
    buf->a = realloc(buf->a, (buf->alloc = buf->used) * buf->datasize);
}

void buffer_out(buffer *buf) {
    if (*buf == NULL || (*buf)->signature != BUFFER_SIGNATURE)
        return; /* Fail quietly */

    free((*buf)->a);
    buffer_reset(*buf);
    (*buf)->signature = BUFFER_SIGNATURE;
    free(*buf);
    *buf = NULL;
}

void buffer_sort(buffer buf, int (*comp)(const void *, const void *)) {
    buffer_assert(buf);
    qsort(buf->a, buf->used, buf->datasize, comp);
}

/* --- Getters --- */

size_t buffer_num(buffer buf) {
    buffer_assert(buf);
    return buf->used;
}

size_t buffer_empty(buffer buf) { return buffer_num(buf) == 0; }

void *_buffer_front(buffer buf) {
    buffer_assert(buf);
    if (buf->used == 0)
        throw("buffer_front called on empty buffer");
    return buf->a;
}

void *_buffer_back(buffer buf) {
    buffer_assert(buf);
    if (buf->used == 0)
        throw("buffer_back called on empty buffer");
    return (uint8_t *)buf->a + (buf->used - 1) * buf->datasize;
}

size_t buffer_find(buffer buf, void *element) {
    size_t i;
    for (i = 0; i < buf->used; ++i) {
        const void *e = (uint8_t *)buf->a + i * buf->datasize;
        if (0 == memcmp(e, element, buf->datasize))
            return i;
    }
    return BUFFER_NOT_FOUND;
}
