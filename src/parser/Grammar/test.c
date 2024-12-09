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

enum TEST2 {
    TEST2_T_NULL,
    TEST2_T_c,
    TEST2_T_d,
    TEST2_T_NTOKENS,

    TEST2_NT_S,
    TEST2_NT_C,
    TEST2_NSYM
};

#define GETAMAP(X, M)                                                          \
    do {                                                                       \
        set *ptr;                                                              \
        x = (X);                                                               \
        ptr = map_get((M), &x, set);                                           \
        if (!ptr)                                                              \
            throw("Not a thing: " #X);                                         \
        s = *ptr;                                                              \
    } while (0)
#define GET_FIRSTS(X) GETAMAP(X, g.firsts)
#define GET_FOLLOWS(X) GETAMAP(X, g.follows);

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

#define TEST_NUMLOOK(N) assert(set_num(item.look) == (N))
#define TEST_HASLOOK(X)                                                        \
    do {                                                                       \
        x = (X);                                                               \
        assert(set_has(item.look, &x));                                        \
    } while (0)

#define RECYCLE_SET_CLOSURE                                                    \
    do {                                                                       \
        size_t i;                                                              \
        for (i = 0; i < set_num(s); ++i) {                                     \
            Item *aux = buffer_get(s, i, Item);                                \
            set_out(&aux->look);                                               \
        }                                                                      \
        set_out(&s);                                                           \
    } while (0)

void test_lalr(void) {
    Grammar g; /* recycled */
    set s;     /* recycled */
    size_t x;  /* recycled */
    Item item; /* recycled */

    /*
        Test FIRST and FOLLOW: non-tricky grammar
        Source: Dragon Book, Example 4.17 (Grammar 4.11)

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
    Grammar_compute_follows(&g);

    /* FIRST(E) = { (, id } */
    GET_FIRSTS(TEST0_NT_E);
    TEST_NUM(2);
    TEST_HAS(TEST0_T_opar);
    TEST_HAS(TEST0_T_id);
    /* FIRST(E') = { +, epsilon } */
    GET_FIRSTS(TEST0_NT_Ep);
    TEST_NUM(2);
    TEST_HAS(TEST0_T_plus);
    TEST_HAS(EPSILON);
    /* FIRST(T) = { (, id } */
    GET_FIRSTS(TEST0_NT_T);
    TEST_NUM(2);
    TEST_HAS(TEST0_T_opar);
    TEST_HAS(TEST0_T_id);
    /* FIRST(T') = { *, epsilon } */
    GET_FIRSTS(TEST0_NT_Tp);
    TEST_NUM(2);
    TEST_HAS(TEST0_T_star);
    TEST_HAS(EPSILON);
    /* FIRST(F) = { (, id } */
    GET_FIRSTS(TEST0_NT_F);
    TEST_NUM(2);
    TEST_HAS(TEST0_T_opar);
    TEST_HAS(TEST0_T_id);

    /* FOLLOW(E) = { $, ) } */
    GET_FOLLOWS(TEST0_NT_E);
    TEST_NUM(2);
    TEST_HAS(TEST0_NSYM);
    TEST_HAS(TEST0_T_cpar);
    /* FOLLOW(E') = { $, ) } */
    GET_FOLLOWS(TEST0_NT_Ep);
    TEST_NUM(2);
    TEST_HAS(TEST0_NSYM);
    TEST_HAS(TEST0_T_cpar);
    /* FOLLOW(T) = { +, $, ) } */
    GET_FOLLOWS(TEST0_NT_T);
    TEST_NUM(3);
    TEST_HAS(TEST0_T_plus);
    TEST_HAS(TEST0_NSYM);
    TEST_HAS(TEST0_T_cpar);
    /* FOLLOW(T') = { +, $, ) } */
    GET_FOLLOWS(TEST0_NT_Tp);
    TEST_NUM(3);
    TEST_HAS(TEST0_T_plus);
    TEST_HAS(TEST0_NSYM);
    TEST_HAS(TEST0_T_cpar);
    /* FOLLOW(F) = { *, +, $, ) } */
    GET_FOLLOWS(TEST0_NT_F);
    TEST_NUM(4);
    TEST_HAS(TEST0_T_star);
    TEST_HAS(TEST0_T_plus);
    TEST_HAS(TEST0_NSYM);
    TEST_HAS(TEST0_T_cpar);
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
    Grammar_compute_follows(&g);

    /* FIRST(E) = { a, b } <- Tricky: no epsilon! */
    GET_FIRSTS(TEST1_NT_E);
    TEST_NUM(2);
    TEST_HAS(TEST1_T_a);
    TEST_HAS(TEST1_T_b);
    TEST_NOT(EPSILON);
    /* FIRST(A) = { a, epsilon } */
    GET_FIRSTS(TEST1_NT_A);
    TEST_NUM(2);
    TEST_HAS(TEST1_T_a);
    TEST_HAS(EPSILON);
    /* FIRST(B) = { b } */
    GET_FIRSTS(TEST1_NT_B);
    TEST_NUM(1);
    TEST_HAS(TEST1_T_b);
    TEST_NOT(EPSILON);

    /* FOLLOW(E) = { $ } */
    GET_FOLLOWS(TEST1_NT_E);
    TEST_NUM(1);
    TEST_HAS(TEST1_NSYM);
    /* FOLLOW(A) = { b } */
    GET_FOLLOWS(TEST1_NT_A);
    TEST_NUM(1);
    TEST_HAS(TEST1_T_b);
    /* FOLLOW(B) = { $ } */
    GET_FOLLOWS(TEST1_NT_B);
    TEST_NUM(1);
    TEST_HAS(TEST1_NSYM);
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
    Grammar_compute_follows(&g);

    /* FIRST(E) = { a, b, epsilon } */
    GET_FIRSTS(TEST1_NT_E);
    TEST_NUM(3);
    TEST_HAS(TEST1_T_a);
    TEST_HAS(TEST1_T_b);
    TEST_HAS(EPSILON);
    /* FIRST(A) = { a, epsilon } */
    GET_FIRSTS(TEST1_NT_A);
    TEST_NUM(2);
    TEST_HAS(TEST1_T_a);
    TEST_HAS(EPSILON);
    /* FIRST(B) = { b, epsilon } */
    GET_FIRSTS(TEST1_NT_B);
    TEST_NUM(2);
    TEST_HAS(TEST1_T_b);
    TEST_HAS(EPSILON);

    /* FOLLOW(E) = { $ } */
    GET_FOLLOWS(TEST1_NT_E);
    TEST_NUM(1);
    TEST_HAS(TEST1_NSYM);
    /* FOLLOW(A) = { b, $ } */
    GET_FOLLOWS(TEST1_NT_A);
    TEST_NUM(2);
    TEST_HAS(TEST1_T_b);
    TEST_HAS(TEST1_NSYM);
    /* FOLLOW(B) = { $ } */
    GET_FOLLOWS(TEST1_NT_B);
    TEST_NUM(1);
    TEST_HAS(TEST1_NSYM);
    Grammar_out(&g);

    /*
        Test CLOSURE
        Source: Dragon Book, Example 4.42

        (S' -> S)
        S -> CC
        C -> cC | d
    */
    Grammar_new(&g, TEST2_T_NTOKENS, TEST2_NSYM, TEST2_NT_S);
    Grammar_add(&g, TEST2_NT_S, 2, TEST2_NT_C, TEST2_NT_C);
    Grammar_add(&g, TEST2_NT_C, 2, TEST2_T_c, TEST2_NT_C);
    Grammar_add(&g, TEST2_NT_C, 1, TEST2_T_d);
    Grammar_augment(&g);
    Grammar_shrink(&g);
    Grammar_compute_firsts(&g);

    /*
        I = {[S' -> ·S, $]}
        I0:
            S' -> ·S, $
            S  -> ·CC, $
            C  -> ·cC, c/d
            C  -> ·d, c/d
    */
    item.prod = 3; /* Augmented start */
    item.dot = 0;
    item.look = NULL;
    set_new(&item.look, sizeof(size_t));
    x = TEST2_NSYM; /* $ */
    set_add(item.look, &x);
    s = Grammar_closure(&g, &item);
    set_out(&item.look);
    TEST_NUM(4);
    /* Hardcoded item index due to bad set implementation */
    /* Hardcoded prod index due to order in Grammar_add above */
    /* S' -> ·S, $ */
    item = *buffer_get(s, 0, Item);
    assert(item.prod == 3); /* Augmented start */
    assert(item.dot == 0);
    TEST_NUMLOOK(1);
    TEST_HASLOOK(TEST2_NSYM);
    /* S -> ·CC, $ */
    item = *buffer_get(s, 1, Item);
    assert(item.prod == 0);
    assert(item.dot == 0);
    TEST_NUMLOOK(1);
    TEST_HASLOOK(TEST2_NSYM);
    /* C -> ·cC, c/d */
    item = *buffer_get(s, 2, Item);
    assert(item.prod == 1);
    assert(item.dot == 0);
    TEST_NUMLOOK(2);
    TEST_HASLOOK(TEST2_T_c);
    TEST_HASLOOK(TEST2_T_d);
    /* C -> ·d, c/d */
    item = *buffer_get(s, 3, Item);
    assert(item.prod == 2);
    assert(item.dot == 0);
    TEST_NUMLOOK(2);
    TEST_HASLOOK(TEST2_T_c);
    TEST_HASLOOK(TEST2_T_d);
    RECYCLE_SET_CLOSURE;

    /*
        A bit more tricky
        I = {[C -> c·C, c/d]}
        I3:
            C -> c·C, c/d
            C -> ·cC, c/d <- Tricky, must perform FIRST(epsilon c/d)
            C -> ·d, c/d
    */
    item.prod = 1; /* order in Grammar_add above */
    item.dot = 1;
    item.look = NULL;
    set_new(&item.look, sizeof(size_t));
    x = TEST2_T_c;
    set_add(item.look, &x);
    x = TEST2_T_d;
    set_add(item.look, &x);
    s = Grammar_closure(&g, &item);
    set_out(&item.look);
    TEST_NUM(3);
    /* C -> c·C, c/d */
    item = *buffer_get(s, 0, Item);
    assert(item.prod == 1);
    assert(item.dot == 1);
    TEST_NUMLOOK(2);
    TEST_HASLOOK(TEST2_T_c);
    TEST_HASLOOK(TEST2_T_d);
    /* C -> ·cC, c/d */
    item = *buffer_get(s, 1, Item);
    assert(item.prod == 1);
    assert(item.dot == 0);
    TEST_NUMLOOK(2);
    TEST_HASLOOK(TEST2_T_c);
    TEST_HASLOOK(TEST2_T_d);
    /* C -> ·d, c/d */
    item = *buffer_get(s, 2, Item);
    assert(item.prod == 2);
    assert(item.dot == 0);
    TEST_NUMLOOK(2);
    TEST_HASLOOK(TEST2_T_c);
    TEST_HASLOOK(TEST2_T_d);
    RECYCLE_SET_CLOSURE;
    Grammar_out(&g);

    printf("NICE!\n");
}
