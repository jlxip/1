#ifndef EASY_EDGE_HELPERS_H
#define EASY_EDGE_HELPERS_H

#include "../../easy.h"
#include "../../src/Grammar.h"
#include "../../src/internal.h"
#include <common.h>

#define MAKE_GENERIC(X)                                                        \
    ((Grammar *)grammar(tokens_##X, nts_##X, grammar_##X, "S"))

/* For first */
#define TEST_NUM(N) assert(set_num(s) == (N))
#define TEST_HAS(X)                                                            \
    do {                                                                       \
        x = (X);                                                               \
        assert(set_has(s, &x));                                                \
    } while (0)

/* For goto */
#define TEST_ITEM                                                              \
    do {                                                                       \
        assert(set_has(s, &item));                                             \
        destroy_item(&item);                                                   \
    } while (0)

/* For collection */
#define CHECK_STATE                                                            \
    do {                                                                       \
        size_t found = 0, i;                                                   \
        for (i = 0; i < buffer_num(g->collection); ++i) {                      \
            if (equal_set(buffer_get(g->collection, i, set), &s)) {            \
                found = 1;                                                     \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        assert(found);                                                         \
    } while (0)

/* Printing functions */
void print_item(Item *item);
void print_set_of_items(set s);

#endif
