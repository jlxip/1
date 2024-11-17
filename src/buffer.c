#include <buffer.h>
#include <common.h>
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

void _buffer_push(buffer buf, void *element) {
    char *target = NULL;
    buffer_assert(buf);
    if (buf->used == buf->alloc)
        buffer_2x(buf);
    target = (char *)buf->a + buf->used++ * buf->datasize;
    memcpy(target, element, buf->datasize);
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
    (*buf)->signature = 0xDEADBEEF;
    free(*buf);
    *buf = NULL;
}

void buffer_sort(buffer buf, int (*comp)(const void *, const void *)) {
    buffer_assert(buf);
    qsort(buf->a, buf->used, buf->datasize, comp);
}
