#include "../easy.h"
#include "../src/Grammar.h"
#include "grammars.h"
#include <common.h>

/* As weirdly written as possible to test grammar() */
static const char grammar_basic_weird[] = "    \n"
                                          "  S  ->  C C  \n"
                                          "C->c C| d\n \n\n";

/* Check G is the grammar defined in test1 (and test2) */
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

    g = (Grammar *)grammar(tokens_basic, nts_basic, grammar_basic, "S");
    check(g);
    Grammar_out(g);
    free(g);

    g = (Grammar *)grammar(tokens_basic, nts_basic, grammar_basic_weird, "S");
    check(g);
    Grammar_out(g);
    free(g);

    g = (Grammar *)grammar(tokens_epsilon, nts_epsilon, grammar_epsilon, "S");
    assert(g->ntok == 1);
    assert(g->nsym == 3);
    assert(buffer_num(g->g) == 1);
    /* S -> epsilon */
    prod = buffer_get(g->g, 0, Production);
    assert(prod->lhs == 2);
    assert(buffer_num(prod->rhs) == 0);
    Grammar_out(g);
    free(g);

    g = (Grammar *)grammar(tokens_lr, nts_lr, grammar_lr, "S");
    assert(g->ntok == 4);
    assert(g->nsym == 8);
    assert(buffer_num(g->g) == 5);
    /* S -> L = R */
    prod = buffer_get(g->g, 0, Production);
    assert(prod->lhs == 5);
    assert(buffer_num(prod->rhs) == 3);
    assert(*buffer_get(prod->rhs, 0, symbol) == 6);
    assert(*buffer_get(prod->rhs, 1, symbol) == 1);
    assert(*buffer_get(prod->rhs, 2, symbol) == 7);
    /* S -> R */
    prod = buffer_get(g->g, 1, Production);
    assert(prod->lhs == 5);
    assert(buffer_num(prod->rhs) == 1);
    assert(*buffer_get(prod->rhs, 0, symbol) == 7);
    /* L -> * R */
    prod = buffer_get(g->g, 2, Production);
    assert(prod->lhs == 6);
    assert(buffer_num(prod->rhs) == 2);
    assert(*buffer_get(prod->rhs, 0, symbol) == 2);
    assert(*buffer_get(prod->rhs, 1, symbol) == 7);
    /* L -> id */
    prod = buffer_get(g->g, 3, Production);
    assert(prod->lhs == 6);
    assert(buffer_num(prod->rhs) == 1);
    assert(*buffer_get(prod->rhs, 0, symbol) == 3);
    /* R -> L */
    prod = buffer_get(g->g, 4, Production);
    assert(prod->lhs == 7);
    assert(buffer_num(prod->rhs) == 1);
    assert(*buffer_get(prod->rhs, 0, symbol) == 6);

    Grammar_out(g);
    free(g);
}
