#include "walk.h"
#include <stdlib.h>

size_t hash_type(const void *x) {
    const Type *type = (const Type *)x;
    return hash_buffer(type, sizeof(Type));
}

size_t hash_declaration(const void *x) {
    size_t ret;
    const Declaration *dec = (const Declaration *)x;
    ret = hash_size_t(&dec->lineno);
    ret = combine_hashes(ret, hash_type(&dec->type));
    ret = combine_hashes(ret, hash_bool(&dec->mut));
    return ret;
}

size_t equal_declaration(const void *ptra, const void *ptrb) {
    const Declaration *a = (const Declaration *)ptra;
    const Declaration *b = (const Declaration *)ptrb;
    if (a->lineno != b->lineno)
        return 0;
    if (a->type != b->type)
        return 0;
    return a->mut == b->mut;
}

void *copy_declaration(const void *x) {
    const Declaration *dec = (const Declaration *)x;
    Declaration *ret = malloc(sizeof(Declaration));
    *ret = *dec;
    return ret;
}

void destroy_declaration(void *x) { (void)x; }
