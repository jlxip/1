#include "Grammar.h"
#include <stdio.h>

enum TEST0 {
    TEST0_T_NULL,
    TEST0_T_plus,
    TEST0_T_star,
    TEST0_T_opar,
    TEST0_T_cpar,
    TEST0_T_id,
    TEST0_T_NTOKENS,

    TEST0_NT_E,
    TEST0_NT_Ep,
    TEST0_NT_T,
    TEST0_NT_Tp,
    TEST0_NT_F,
    TEST0_NSYM
};

enum TEST1 {
    TEST1_T_NULL,
    TEST1_T_a,
    TEST1_T_b,
    TEST1_T_NTOKENS,

    TEST1_NT_E,
    TEST1_NT_A,
    TEST1_NT_B,
    TEST1_NSYM
};

#define GET_FIRSTS(X)                                                          \
    do {                                                                       \
        set *ptr;                                                              \
        x = (X);                                                               \
        ptr = map_get(g.firsts, &x, set);                                      \
        if (!ptr)                                                              \
            throw("Not a thing: " #X);                                         \
        s = *ptr;                                                              \
    } while (0)
#define TEST_NUMBUF(N) assert(set_num(s) == (N))
#define TEST_BUF(X)                                                            \
    do {                                                                       \
        x = (X);                                                               \
        assert(set_has(s, &x));                                                \
    } while (0)
#define TEST_NOTBUF(X)                                                         \
    do {                                                                       \
        x = (X);                                                               \
        assert(!set_has(s, &x));                                               \
    } while (0)

void test_lalr(void) {
    Grammar g; /* recycled */
    set s;     /* recycled */
    size_t x;  /* recycled */

    /*
        Test FIRST and FOLLOW: non-tricky grammar
        Source: Dragon Book, Example 4.17

        E  -> T E'
        E' -> + T E' | epsilon
        T  -> F T'
        T' -> * F T' | epsilon
        F  -> ( E ) | id
    */
    Grammar_new(&g, TEST0_T_NTOKENS, TEST0_NSYM, TEST0_NT_E);
    Grammar_add(&g, TEST0_NT_E, 2, TEST0_NT_T, TEST0_NT_Ep);
    Grammar_add(&g, TEST0_NT_Ep, 3, TEST0_T_plus, TEST0_NT_T, TEST0_NT_Ep);
    Grammar_add(&g, TEST0_NT_Ep, EPSILON);
    Grammar_add(&g, TEST0_NT_T, 2, TEST0_NT_F, TEST0_NT_Tp);
    Grammar_add(&g, TEST0_NT_Tp, 3, TEST0_T_star, TEST0_NT_F, TEST0_NT_Tp);
    Grammar_add(&g, TEST0_NT_Tp, EPSILON);
    Grammar_add(&g, TEST0_NT_F, 3, TEST0_T_opar, TEST0_NT_E, TEST0_T_cpar);
    Grammar_add(&g, TEST0_NT_F, 1, TEST0_T_id);
    Grammar_augment(&g);
    Grammar_shrink(&g);
    Grammar_compute_firsts(&g);

    /* FIRST(E) = { (, id } */
    GET_FIRSTS(TEST0_NT_E);
    TEST_NUMBUF(2);
    TEST_BUF(TEST0_T_opar);
    TEST_BUF(TEST0_T_id);
    /* FIRST(E') = { +, epsilon } */
    GET_FIRSTS(TEST0_NT_Ep);
    TEST_NUMBUF(2);
    TEST_BUF(TEST0_T_plus);
    TEST_BUF(EPSILON);
    /* FIRST(T) = { (, id } */
    GET_FIRSTS(TEST0_NT_T);
    TEST_NUMBUF(2);
    TEST_BUF(TEST0_T_opar);
    TEST_BUF(TEST0_T_id);
    /* FIRST(T') = { *, epsilon } */
    GET_FIRSTS(TEST0_NT_Tp);
    TEST_NUMBUF(2);
    TEST_BUF(TEST0_T_star);
    TEST_BUF(EPSILON);
    /* FIRST(F) = { (, id } */
    GET_FIRSTS(TEST0_NT_F);
    TEST_NUMBUF(2);
    TEST_BUF(TEST0_T_opar);
    TEST_BUF(TEST0_T_id);
    Grammar_out(&g);

    /*
        Test FIRST and FOLLOW: special case with epsilon #1

        E -> A B
        A -> a | epsilon
        B -> b
    */
    Grammar_new(&g, TEST1_T_NTOKENS, TEST1_NSYM, TEST1_NT_E);
    Grammar_add(&g, TEST1_NT_E, 2, TEST1_NT_A, TEST1_NT_B);
    Grammar_add(&g, TEST1_NT_A, 1, TEST1_T_a);
    Grammar_add(&g, TEST1_NT_A, EPSILON);
    Grammar_add(&g, TEST1_NT_B, 1, TEST1_T_b);
    Grammar_augment(&g);
    Grammar_shrink(&g);
    Grammar_compute_firsts(&g);

    /* FIRST(E) = { a, b } <- Tricky: no epsilon! */
    GET_FIRSTS(TEST1_NT_E);
    TEST_NUMBUF(2);
    TEST_BUF(TEST1_T_a);
    TEST_BUF(TEST1_T_b);
    TEST_NOTBUF(EPSILON);
    /* FIRST(A) = { a, epsilon } */
    GET_FIRSTS(TEST1_NT_A);
    TEST_NUMBUF(2);
    TEST_BUF(TEST1_T_a);
    TEST_BUF(EPSILON);
    /* FIRST(B) = { b } */
    GET_FIRSTS(TEST1_NT_B);
    TEST_NUMBUF(1);
    TEST_BUF(TEST1_T_b);
    TEST_NOTBUF(EPSILON);
    Grammar_out(&g);

    /*
        Test FIRST and FOLLOW: special case with epsilon #2

        E -> A B
        A -> a | epsilon
        B -> b | epsilon
    */
    Grammar_new(&g, TEST1_T_NTOKENS, TEST1_NSYM, TEST1_NT_E);
    Grammar_add(&g, TEST1_NT_E, 2, TEST1_NT_A, TEST1_NT_B);
    Grammar_add(&g, TEST1_NT_A, 1, TEST1_T_a);
    Grammar_add(&g, TEST1_NT_A, EPSILON);
    Grammar_add(&g, TEST1_NT_B, 1, TEST1_T_b);
    Grammar_add(&g, TEST1_NT_B, EPSILON);
    Grammar_augment(&g);
    Grammar_shrink(&g);
    Grammar_compute_firsts(&g);

    /* FIRST(E) = { a, b, epsilon } */
    GET_FIRSTS(TEST1_NT_E);
    TEST_NUMBUF(3);
    TEST_BUF(TEST1_T_a);
    TEST_BUF(TEST1_T_b);
    TEST_BUF(EPSILON);
    /* FIRST(A) = { a, epsilon } */
    GET_FIRSTS(TEST1_NT_A);
    TEST_NUMBUF(2);
    TEST_BUF(TEST1_T_a);
    TEST_BUF(EPSILON);
    /* FIRST(B) = { b, epsilon } */
    GET_FIRSTS(TEST1_NT_B);
    TEST_NUMBUF(2);
    TEST_BUF(TEST1_T_b);
    TEST_BUF(EPSILON);
    Grammar_out(&g);

    printf("NICE!\n");
}
