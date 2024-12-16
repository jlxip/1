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
    if ((X) == NULL || (X)->signature != BUFFER_SIGNATURE)                     \
        throw("called on uninitialized buffer");

void buffer_push(buffer buf, const void *element) {
    char *target = NULL;
    buffer_assert(buf);
    if (buf->used == buf->alloc)
        buffer_2x(buf);
    target = (char *)buf->a + buf->used++ * buf->datasize;
    memcpy(target, element, buf->datasize);
}

void buffer_set(buffer buf, size_t idx, const void *element) {
    char *target = NULL;
    buffer_assert(buf);
    if (idx >= buf->used)
        throw("buffer_set with invalid index");
    target = (char *)buf->a + idx * buf->datasize;
    memcpy(target, element, buf->datasize);
}

void buffer_pop(buffer buf) {
    buffer_assert(buf);
    if (buf->used == 0)
        throw("buffer_pop called on empty buffer");
    buf->used--;
}

void buffer_remove(buffer buf, size_t idx) {
    uint8_t *data = (uint8_t *)buf->a;
    size_t src, dst, howmany;

    buffer_assert(buf);
    if (idx >= buf->used)
        throw("invalid idx in call to buffer_remove");

    dst = idx * buf->datasize;
    src = dst + buf->datasize;
    howmany = buf->used - idx - 1;
    memmove(data + dst, data + src, howmany * buf->datasize);
    buf->used--;
}

buffer buffer_copy(const buffer buf) {
    buffer new = NULL;
    size_t i;

    buffer_assert(buf);
    buffer_new(&new, buf->datasize);
    for (i = 0; i < buffer_num(buf); ++i)
        buffer_push(new, buffer_get(buf, i, void));
    return new;
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
    (*buf)->signature = 0;
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

size_t buffer_find(buffer buf, const void *element) {
    size_t i;
    buffer_assert(buf);
    for (i = 0; i < buf->used; ++i) {
        const void *e = (uint8_t *)buf->a + i * buf->datasize;
        if (0 == memcmp(e, element, buf->datasize))
            return i;
    }
    return NOT_FOUND;
}

void *_buffer_get(buffer buf, size_t idx) {
    buffer_assert(buf);
    if (buf->used <= idx)
        throw("buffer_get called on bad index");
    return (uint8_t *)buf->a + idx * buf->datasize;
}
