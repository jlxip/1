#include "../easy.h"
#include "../src/internal.h"
#include "grammars.h"
#include <common.h>

#define TEST_NUM(N) assert(set_num(s) == (N))
#define TEST_HAS(X)                                                            \
    do {                                                                       \
        x = (X);                                                               \
        assert(set_has(s, &x));                                                \
    } while (0)
#define TEST_NOT(X)                                                            \
    do {                                                                       \
        x = (X);                                                               \
        assert(!set_has(s, &x));                                               \
    } while (0)

static void test_forfirst(void) {
    Grammar *g;
    size_t x;
    set s;

    g = grammar(tokens_forfirst, nts_forfirst, grammar_forfirst, "E");

    /* FIRST(E) = { (, id } */
    s = FIRST(g, 7);
    TEST_NUM(2);
    TEST_HAS(3);
    TEST_HAS(5);
    /* FIRST(E') = { +, epsilon } */
    s = FIRST(g, 8);
    TEST_NUM(2);
    TEST_HAS(1);
    TEST_HAS(0);
    /* FIRST(T) = { (, id } */
    s = FIRST(g, 9);
    TEST_NUM(2);
    TEST_HAS(3);
    TEST_HAS(5);
    /* FIRST(T') = { *, epsilon } */
    s = FIRST(g, 10);
    TEST_NUM(2);
    TEST_HAS(2);
    TEST_HAS(0);
    /* FIRST(F) = { (, id } */
    s = FIRST(g, 11);
    TEST_NUM(2);
    TEST_HAS(3);
    TEST_HAS(5);

    Grammar_out(g);
    free(g);
}

/* -------------------------------------------------------------------------- */

static void test_forfirst2(void) {
    Grammar *g;
    size_t x;
    set s;

    g = grammar(tokens_forfirst2, nts_forfirst2, grammar_forfirst2, "E");

    /* FIRST(E) = { a, b } <- Tricky: no epsilon! */
    s = FIRST(g, 4);
    TEST_NUM(2);
    TEST_HAS(1);
    TEST_HAS(2);
    TEST_NOT(0);
    /* FIRST(A) = { a, epsilon } */
    s = FIRST(g, 5);
    TEST_NUM(2);
    TEST_HAS(1);
    TEST_HAS(0);
    /* FIRST(B) = { b } */
    s = FIRST(g, 6);
    TEST_NUM(1);
    TEST_HAS(2);
    TEST_NOT(0);

    Grammar_out(g);
    free(g);
}

/* -------------------------------------------------------------------------- */

static void test_forfirst3(void) {
    Grammar *g;
    size_t x;
    set s;

    g = grammar(tokens_forfirst3, nts_forfirst3, grammar_forfirst3, "E");

    /* FIRST(E) = { a, b, epsilon } */
    s = FIRST(g, 4);
    TEST_NUM(3);
    TEST_HAS(1);
    TEST_HAS(2);
    TEST_HAS(EPSILON);
    /* FIRST(A) = { a, epsilon } */
    s = FIRST(g, 5);
    TEST_NUM(2);
    TEST_HAS(1);
    TEST_HAS(0);
    /* FIRST(B) = { b, epsilon } */
    s = FIRST(g, 6);
    TEST_NUM(2);
    TEST_HAS(2);
    TEST_HAS(0);

    Grammar_out(g);
    free(g);
}

/* -------------------------------------------------------------------------- */

static void test_random(void) {
    Grammar *g;
    size_t x;
    set s;

    g = grammar(tokens_random, nts_random, grammar_random, "S");

    /* FIRST(S) = { a, b } */
    s = FIRST(g, 6);
    TEST_NUM(2);
    TEST_HAS(3);
    TEST_HAS(4);
    /* FIRST(T) = { a, b } */
    s = FIRST(g, 7);
    TEST_NUM(2);
    TEST_HAS(3);
    TEST_HAS(4);
    /* FIRST(F) = { a, b } */
    s = FIRST(g, 8);
    TEST_NUM(2);
    TEST_HAS(3);
    TEST_HAS(4);

    Grammar_out(g);
    free(g);
}

/* -------------------------------------------------------------------------- */

static void test_wtf(void) {
    Grammar *g;
    size_t x;
    set s;

    g = grammar(tokens_wtf, nts_wtf, grammar_wtf, "S");

    /* FIRST(S) = { a, c, e, x } */
    s = FIRST(g, 8);
    TEST_NUM(4);
    TEST_HAS(1);
    TEST_HAS(3);
    TEST_HAS(5);
    TEST_HAS(6);
    /* FIRST(A) = { a, EPSILON } */
    s = FIRST(g, 9);
    TEST_NUM(2);
    TEST_HAS(1);
    TEST_HAS(0);
    /* FIRST(B) = { x } */
    s = FIRST(g, 10);
    TEST_NUM(1);
    TEST_HAS(6);
    /* FIRST(C) = { c, e } */
    s = FIRST(g, 11);
    TEST_NUM(2);
    TEST_HAS(3);
    TEST_HAS(5);

    Grammar_out(g);
    free(g);
}

/* -------------------------------------------------------------------------- */

void test_lalr_first(void) {
    test_forfirst();
    test_forfirst2();
    test_forfirst3();
    test_random();
    test_wtf();
}
