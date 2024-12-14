#include <ds/functors.h>
#include <stdlib.h>
#include <string.h>

size_t hash_buffer(const void *ptr, size_t sz) {
    /* 7 and 31 come from java */
    const unsigned char *x = (const unsigned char *)ptr;
    size_t ret = 7;
    while (--sz) {
        unsigned char c = *x++;
        ret = ret * 31 + c;
    }

    return ret;
}

size_t combine_hashes(size_t a, size_t b) {
    /*
        I made this up
        I don't know if it's good, but it's not terrible
        It's better than XOR tho, since it's order-sensitive
    */
    size_t hi, lo, ret;
    hi = a >> (8 * sizeof(size_t) / 2);
    lo = a << (8 * sizeof(size_t) / 2);
    ret = (hi | lo) ^ b;
    return hash_size_t(&ret);
}

/* --- Commons --- */

size_t hash_string(const void *ptr) {
    /* Implement hash_buffer again */
    const char *s = (const char *)ptr;
    size_t ret = 7;
    char c;
    while ((c = *s++))
        ret = ret * 31 + c;
    return ret;
}

size_t equal_string(const void *ptra, const void *ptrb) {
    const char *a = (const char *)ptra;
    const char *b = (const char *)ptrb;
    return strcmp(a, b) == 0;
}

void *copy_string(const void *ptra) {
    const char *a = (const char *)ptra;
    char *b;
    size_t sz;

    sz = strlen(a);
    b = malloc(sz + 1);
    memcpy(b, a, sz);
    b[sz] = '\0';
    return b;
}

void destroy_string(void *ptr) { (void)ptr; } /* Trivial */

size_t hash_size_t(const void *x) {
    /*
        There are many ways to do this and none of them are purely portable.
        All hash functions for integers I've seen rely on some large
        magic number, which destroys portability since sizeof(size_t) should
        not matter for this. So, this is the best I've come up with.
    */
    return hash_buffer(x, sizeof(size_t));
}

size_t equal_size_t(const void *ptra, const void *ptrb) {
    const size_t *a = (const size_t *)ptra;
    const size_t *b = (const size_t *)ptrb;
    return *a == *b;
}

void *copy_size_t(const void *ptra) {
    const size_t *a = (const size_t *)ptra;
    size_t *b = malloc(sizeof(size_t));
    *b = *a;
    return b;
}

void destroy_size_t(void *ptr) { (void)ptr; } /* Trivial */
