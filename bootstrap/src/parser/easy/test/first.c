#include "../easy.h"
#include "../src/Grammar.h"
#include <common.h>

#define GET_FIRST(X)                                                           \
    do {                                                                       \
        set *ptr;                                                              \
        x = (X);                                                               \
        ptr = map_get(g->firsts, &x, set);                                     \
        if (!ptr)                                                              \
            throw("Not a thing: " #X);                                         \
        s = *ptr;                                                              \
    } while (0)

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

/*
    Non-tricky grammar
    Source: Dragon Book, Example 4.17 (Grammar 4.11)

    E  -> T E'
    E' -> + T E' | epsilon
    T  -> F T'
    T' -> * F T' | epsilon
    F  -> ( E ) | id
*/
static const char *tokens1[] = {"+", "*", "(", ")", "id", NULL};
static const char *nts1[] = {"E", "Ep", "T", "Tp", "F", NULL};
static const char grammar1[] = "E  -> T Ep\n"
                               "Ep -> + T Ep | EPSILON\n"
                               "T  -> F Tp\n"
                               "Tp -> * F Tp | EPSILON\n"
                               "F  -> ( E ) | id\n";

static void test1(void) {
    Grammar *g;
    size_t x;
    set s;

    g = grammar(tokens1, nts1, grammar1, "E");
    Grammar_augment(g);
    Grammar_compute_firsts(g);

    /* FIRST(E) = { (, id } */
    GET_FIRST(7);
    TEST_NUM(2);
    TEST_HAS(3);
    TEST_HAS(5);
    /* FIRST(E') = { +, epsilon } */
    GET_FIRST(8);
    TEST_NUM(2);
    TEST_HAS(1);
    TEST_HAS(0);
    /* FIRST(T) = { (, id } */
    GET_FIRST(9);
    TEST_NUM(2);
    TEST_HAS(3);
    TEST_HAS(5);
    /* FIRST(T') = { *, epsilon } */
    GET_FIRST(10);
    TEST_NUM(2);
    TEST_HAS(2);
    TEST_HAS(0);
    /* FIRST(F) = { (, id } */
    GET_FIRST(11);
    TEST_NUM(2);
    TEST_HAS(3);
    TEST_HAS(5);

    Grammar_out(g);
    free(g);
}

/* -------------------------------------------------------------------------- */

/*
    Special case with epsilon #1

    E -> A B
    A -> a | epsilon
    B -> b
*/
static const char *tokens2[] = {"a", "b", NULL};
static const char *nts2[] = {"E", "A", "B", NULL};
static const char *grammar2 = "E -> A B\n"
                              "A -> a | EPSILON\n"
                              "B -> b\n";

static void test2(void) {
    Grammar *g;
    size_t x;
    set s;

    g = grammar(tokens2, nts2, grammar2, "E");
    Grammar_augment(g);
    Grammar_compute_firsts(g);

    /* FIRST(E) = { a, b } <- Tricky: no epsilon! */
    GET_FIRST(4);
    TEST_NUM(2);
    TEST_HAS(1);
    TEST_HAS(2);
    TEST_NOT(0);
    /* FIRST(A) = { a, epsilon } */
    GET_FIRST(5);
    TEST_NUM(2);
    TEST_HAS(1);
    TEST_HAS(0);
    /* FIRST(B) = { b } */
    GET_FIRST(6);
    TEST_NUM(1);
    TEST_HAS(2);
    TEST_NOT(0);

    Grammar_out(g);
    free(g);
}

/* -------------------------------------------------------------------------- */

/*
    Special case with epsilon #2

    E -> A B
    A -> a | epsilon
    B -> b | epsilon
*/

static const char *grammar3 = "E -> A B\n"
                              "A -> a | EPSILON\n"
                              "B -> b | EPSILON\n";

static void test3(void) {
    Grammar *g;
    size_t x;
    set s;

    g = grammar(tokens2, nts2, grammar3, "E");
    Grammar_augment(g);
    Grammar_compute_firsts(g);

    /* FIRST(E) = { a, b, epsilon } */
    GET_FIRST(4);
    TEST_NUM(3);
    TEST_HAS(1);
    TEST_HAS(2);
    TEST_HAS(EPSILON);
    /* FIRST(A) = { a, epsilon } */
    GET_FIRST(5);
    TEST_NUM(2);
    TEST_HAS(1);
    TEST_HAS(0);
    /* FIRST(B) = { b, epsilon } */
    GET_FIRST(6);
    TEST_NUM(2);
    TEST_HAS(2);
    TEST_HAS(0);

    Grammar_out(g);
    free(g);
}

/* -------------------------------------------------------------------------- */

void test_lalr_first(void) {
    test1();
    test2();
    test3();
}
