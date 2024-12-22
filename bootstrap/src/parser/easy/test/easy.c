#include "../easy.h"
#include "../src/Grammar.h"
#include <common.h>

static const char *tokens[] = {"c", "d", NULL};
static const char *nts[] = {"S", "C", NULL};

/* A regular one */
static const char test1[] = "S -> C C\n"
                            "C -> c C | d\n";

/* As weirdly written as possible to test grammar() */
static const char test2[] = "    \n"
                            "  S  ->  C C  \n"
                            "C->c C| d\n \n\n";

/* Epsilon */
static const char *tokens3[] = {NULL};
static const char *nts3[] = {"S", NULL};
static const char test3[] = "S -> EPSILON\n";

/* Check G is the grammar defined above */
static void check(Grammar *g) {
    const Production *prod;

    assert(g->ntok == 3);
    assert(g->nsym == 6);
    assert(buffer_num(g->g) == 3);
    /* [0] S -> C C */
    prod = buffer_get(g->g, 0, Production);
    assert(prod->lhs == 4);
    assert(buffer_num(prod->rhs) == 2);
    assert(*buffer_get(prod->rhs, 0, symbol) == 5);
    assert(*buffer_get(prod->rhs, 1, symbol) == 5);
    /* [1] C -> c C */
    prod = buffer_get(g->g, 1, Production);
    assert(prod->lhs == 5);
    assert(buffer_num(prod->rhs) == 2);
    assert(*buffer_get(prod->rhs, 0, symbol) == 1);
    assert(*buffer_get(prod->rhs, 1, symbol) == 5);
    /* [2] C -> d */
    prod = buffer_get(g->g, 2, Production);
    assert(prod->lhs == 5);
    assert(buffer_num(prod->rhs) == 1);
    assert(*buffer_get(prod->rhs, 0, symbol) == 2);
}

void test_lalr_easy(void) {
    Grammar *g;
    const Production *prod;

    g = (Grammar *)grammar(tokens, nts, test1, "S");
    check(g);
    Grammar_out(g);
    free(g);

    g = (Grammar *)grammar(tokens, nts, test2, "S");
    check(g);
    Grammar_out(g);
    free(g);

    g = (Grammar *)grammar(tokens3, nts3, test3, "S");
    assert(g->ntok == 1);
    assert(g->nsym == 3);
    assert(buffer_num(g->g) == 1);
    /* S -> epsilon */
    prod = buffer_get(g->g, 0, Production);
    assert(prod->lhs == 2);
    assert(buffer_num(prod->rhs) == 0);
    Grammar_out(g);
    free(g);
}
