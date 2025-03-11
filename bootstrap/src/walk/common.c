#include "walk.h"
#include <stdlib.h>

size_t hash_declaration(const void *x) {
    const Declaration *dec = (const Declaration *)x;
    return hash_size_t(&dec->lineno);
}

size_t equal_declaration(const void *ptra, const void *ptrb) {
    const Declaration *a = (const Declaration *)ptra;
    const Declaration *b = (const Declaration *)ptrb;
    return a->lineno == b->lineno;
}

void *copy_declaration(const void *x) {
    const Declaration *dec = (const Declaration *)x;
    Declaration *ret = malloc(sizeof(Declaration));
    *ret = *dec;
    return ret;
}

void destroy_declaration(void *x) { (void)x; }
