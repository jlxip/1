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

#define TEST_ITEM                                                              \
    do {                                                                       \
        assert(set_has(s, &item));                                             \
        destroy_item(&item);                                                   \
    } while (0)

#define CHECK_STATE                                                            \
    do {                                                                       \
        size_t found = 0, i;                                                   \
        for (i = 0; i < buffer_num(g.collection); ++i) {                       \
            if (equal_set(buffer_get(g.collection, i, set), &s)) {             \
                found = 1;                                                     \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        assert(found);                                                         \
    } while (0)

void test_lalr(void) {
    Grammar g; /* recycled */
    set s;     /* recycled */
    set s2;    /* recycled */
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
        I0: S' -> ·S, $
            S  -> ·CC, $
            C  -> ·cC, c/d
            C  -> ·d, c/d
    */
    item = Item_new(3 /* S' -> S */, 0 /* ·S */, 1, TEST2_NSYM /* $ */);
    s = Grammar_closure(&g, &item);
    TEST_NUM(4);
    /* Hardcoded prod index due to order in Grammar_add above */
    /* S' -> ·S, $ */
    /* Item already set */
    TEST_ITEM;
    /* S -> ·CC, $ */
    item = Item_new(0 /* S -> CC */, 0 /* ·CC */, 1, TEST2_NSYM /* $ */);
    TEST_ITEM;
    /* C -> ·cC, c/d */
    item = Item_new(1 /* C -> cC */, 0 /* ·cC */, 2, TEST2_T_c, TEST2_T_d);
    TEST_ITEM;
    /* C -> ·d, c/d */
    item = Item_new(2 /* C -> d */, 0 /* ·d */, 2, TEST2_T_c, TEST2_T_d);
    TEST_ITEM;
    set_out(&s);

    /*
        A bit more tricky
        I = {[C -> c·C, c/d]}
        I3: C -> c·C, c/d
            C -> ·cC, c/d <- Tricky, must perform FIRST(epsilon c/d)
            C -> ·d, c/d
    */
    item = Item_new(1 /* C -> cC */, 1 /* c·C */, 2, TEST2_T_c, TEST2_T_d);
    s = Grammar_closure(&g, &item);
    TEST_NUM(3);
    /* C -> c·C, c/d */
    /* Item already set */
    TEST_ITEM;
    /* C -> ·cC, c/d */
    item = Item_new(1 /* C -> cC */, 0 /* ·cC */, 2, TEST2_T_c, TEST2_T_d);
    TEST_ITEM;
    /* C -> ·d, c/d */
    item = Item_new(2 /* C -> d */, 0 /* ·d */, 2, TEST2_T_c, TEST2_T_d);
    TEST_ITEM;
    set_out(&s);

    /*
        Test GOTO
        Same grammar as above

        I0 = closure(I)
        I3 = closure(goto(I0, c))
    */
    item = Item_new(3 /* S' -> S */, 0 /* ·S */, 1, TEST2_NSYM /* $ */);
    s = Grammar_closure(&g, &item);
    destroy_item(&item);
    s2 = Grammar_goto(&g, s, TEST2_T_c);
    set_out(&s);
    s = s2;
    s2 = NULL;

    TEST_NUM(3);
    /* C -> c·C, c/d */
    item = Item_new(1 /* C -> cC */, 1 /* c·C */, 2, TEST2_T_c, TEST2_T_d);
    TEST_ITEM;
    /* C -> ·cC, c/d */
    item = Item_new(1 /* C -> cC */, 0 /* ·cC */, 2, TEST2_T_c, TEST2_T_d);
    TEST_ITEM;
    /* C -> ·d, c/d */
    item = Item_new(2 /* C -> d */, 0 /* ·d */, 2, TEST2_T_c, TEST2_T_d);
    TEST_ITEM;

    /*
        Test GOTO again
        I8 = closure(goto(I3, C))
        I8: C -> cC·, c/d
    */
    s2 = s;
    s = Grammar_goto(&g, s2, TEST2_NT_C);
    set_out(&s2);
    TEST_NUM(1);
    /* C -> cC·, c/d */
    item = Item_new(
        1 /* C -> cC */, END_OF_PRODUCTION /* cC· */, 2, TEST2_T_c, TEST2_T_d);
    TEST_ITEM;
    set_out(&s);

    /*
        Test canonical collection
        Same grammar
    */
    Grammar_compute_collection(&g);
    assert(buffer_num(g.collection) == 10);
    /*
        I0: S' -> ·S, $
            S  -> ·CC, $
            C  -> ·cC, c/d
            C  -> ·d, c/d
    */
    set_new_Item(&s);
    item = Item_new(3 /* S' -> S */, 0 /* ·S */, 1, TEST2_NSYM /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(0 /* S -> CC */, 0 /* ·CC */, 1, TEST2_NSYM /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(1 /* C -> cC */, 0 /* ·cC */, 2, TEST2_T_c, TEST2_T_d);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(2 /* C -> d */, 0 /* ·d */, 2, TEST2_T_c, TEST2_T_d);
    set_add(s, &item);
    destroy_item(&item);
    CHECK_STATE;
    set_out(&s);
    /*
        I1: S' -> S·, $
    */
    set_new_Item(&s);
    item = Item_new(
        3 /* S' -> S */, END_OF_PRODUCTION /* S· */, 1, TEST2_NSYM /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    CHECK_STATE;
    set_out(&s);
    /*
        I2: S -> C·C, $
            C -> ·cC, $
            C -> ·d, $
    */
    set_new_Item(&s);
    item = Item_new(0 /* S -> CC */, 1 /* C·C */, 1, TEST2_NSYM /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(1 /* C -> cC */, 0 /* ·cC */, 1, TEST2_NSYM /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(2 /* C -> d */, 0 /* ·d */, 1, TEST2_NSYM /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    CHECK_STATE;
    set_out(&s);
    /*
        I3: C -> c·C, c/d
            C -> ·cC, c/d
            C -> ·d, c/d
    */
    set_new_Item(&s);
    item = Item_new(1 /* C -> cC */, 1 /* c·C */, 2, TEST2_T_c, TEST2_T_d);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(1 /* C -> cC */, 0 /* ·cC */, 2, TEST2_T_c, TEST2_T_d);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(2 /* C -> d */, 0 /* ·d */, 2, TEST2_T_c, TEST2_T_d);
    set_add(s, &item);
    destroy_item(&item);
    CHECK_STATE;
    set_out(&s);
    /*
        I4: C -> d·, c/d
    */
    set_new_Item(&s);
    item = Item_new(
        2 /* C -> d */, END_OF_PRODUCTION /* d· */, 2, TEST2_T_c, TEST2_T_d);
    set_add(s, &item);
    destroy_item(&item);
    CHECK_STATE;
    set_out(&s);
    /*
        I5: S -> CC·, $
    */
    set_new_Item(&s);
    item = Item_new(
        0 /* S -> CC */, END_OF_PRODUCTION /* CC· */, 1, TEST2_NSYM /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    CHECK_STATE;
    set_out(&s);
    /*
        I6: C -> c·C, $
            C -> ·cC, $
            C -> ·d, $
    */
    set_new_Item(&s);
    item = Item_new(1 /* C -> cC */, 1 /* c·C */, 1, TEST2_NSYM /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(1 /* C -> cC */, 0 /* ·cC */, 1, TEST2_NSYM /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(2 /* C -> d */, 0 /* ·d */, 1, TEST2_NSYM /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    CHECK_STATE;
    set_out(&s);
    /*
        I7: C -> d·, $
    */
    set_new_Item(&s);
    item = Item_new(
        2 /* C -> d */, END_OF_PRODUCTION /* d· */, 1, TEST2_NSYM /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    CHECK_STATE;
    set_out(&s);
    /*
        I8: C -> cC·, c/d
    */
    set_new_Item(&s);
    item = Item_new(
        1 /* C -> cC */, END_OF_PRODUCTION /* cC· */, 2, TEST2_T_c, TEST2_T_d);
    set_add(s, &item);
    destroy_item(&item);
    CHECK_STATE;
    set_out(&s);
    /*
        I9: C -> cC·, $
    */
    set_new_Item(&s);
    item = Item_new(
        1 /* C -> cC */, END_OF_PRODUCTION /* cC· */, 1, TEST2_NSYM /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    CHECK_STATE;
    set_out(&s);
    Grammar_out(&g);

    printf("NICE!\n");
}