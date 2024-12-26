#include "../easy.h"
#include "../src/internal.h"
#include <common.h>

#define TEST_NUM(N) assert(set_num(s) == (N))
#define TEST_ITEM                                                              \
    do {                                                                       \
        assert(set_has(s, &item));                                             \
        destroy_item(&item);                                                   \
    } while (0)

/*
    Source: Dragon Book, Example 4.42

    (S' -> S)
    S -> CC
    C -> cC | d
*/

static const char *tokens[] = {"c", "d", NULL};
static const char *nts[] = {"S", "C", NULL};
static const char test[] = "S -> C C\n"
                           "C -> c C | d\n";

/*
    Source: random internet finding + random LALR(1) Parser Generator
        https://jsmachines.sourceforge.net/machines/lalr1.html

    (S' -> S)
    S -> S + T | T
    T -> TF | F
    F -> F* | a | b
*/
static const char *tokens2[] = {"+", "*", "a", "b", NULL};
static const char *nts2[] = {"S", "T", "F", NULL};
static const char test2[] = "S -> S + T | T\n"
                            "T -> T F | F\n"
                            "F -> F * | a | b\n";

void test_lalr_closure(void) {
    Grammar *g;
    set s;
    set s2 = NULL;
    Item item;

    g = grammar(tokens, nts, test, "S");

    /*
        I = {[S' -> ·S, $]}
        I0: S' -> ·S, $
            S  -> ·CC, $
            C  -> ·cC, c/d
            C  -> ·d, c/d
    */
    item = Item_new(0 /* S' -> S */, 0 /* ·S */, 1, 6 /* $ */);
    s = CLOSURE(g, &item);
    set_new_Item(&s2);
    set_migrate(s2, s);
    s = s2;
    s2 = NULL;
    TEST_NUM(4);
    /* S' -> ·S, $ */
    /* Item already set */
    TEST_ITEM;
    /* S -> ·CC, $ */
    item = Item_new(1 /* S -> CC */, 0 /* ·CC */, 1, 6 /* $ */);
    TEST_ITEM;
    /* C -> ·cC, c/d */
    item = Item_new(2 /* C -> cC */, 0 /* ·cC */, 2, 1, 2);
    TEST_ITEM;
    /* C -> ·d, c/d */
    item = Item_new(3 /* C -> d */, 0 /* ·d */, 2, 1, 2);
    TEST_ITEM;
    set_out(&s);

    /*
        A bit more tricky
        I = {[C -> c·C, c/d]}
        I3: C -> c·C, c/d
            C -> ·cC, c/d <- Tricky, must perform FIRST(epsilon c/d)
            C -> ·d, c/d
    */
    item = Item_new(2 /* C -> cC */, 1 /* c·C */, 2, 1, 2);
    s = CLOSURE(g, &item);
    set_new_Item(&s2);
    set_migrate(s2, s);
    s = s2;
    s2 = NULL;
    TEST_NUM(3);
    /* C -> c·C, c/d */
    /* Item already set */
    TEST_ITEM;
    /* C -> ·cC, c/d */
    item = Item_new(2 /* C -> cC */, 0 /* ·cC */, 2, 1, 2);
    TEST_ITEM;
    /* C -> ·d, c/d */
    item = Item_new(3 /* C -> d */, 0 /* ·d */, 2, 1, 2);
    TEST_ITEM;
    set_out(&s);
    Grammar_out(g);
    free(g);

    /*
        A very tricky one
        I = {[S' -> ·S, $]}
        I0: S' -> ·S, $
            S  -> ·S+T, $/+
            S  -> ·T, $/+
            T  -> ·TF, $/+/a/b
            T  -> ·F, $/+/a/b
            F  -> ·F*, $/+/a/b/ *
            F  -> ·a, $/+/a/b/ *
            F  -> ·b, $/+/a/b/ *
    */
    g = grammar(tokens2, nts2, test2, "S");
    item = Item_new(0 /* S' -> S */, 0 /* ·S */, 1, 9 /* $ */);
    s = CLOSURE(g, &item);
    set_new_Item(&s2);
    set_migrate(s2, s);
    s = s2;
    s2 = NULL;
    TEST_NUM(8);
    /* S' -> ·S, $ */
    /* Item already set */
    TEST_ITEM;
    /* S -> ·S+T, $/+ */
    item = Item_new(1 /* S -> S+T */, 0 /* ·S+T */, 2, 9 /* $ */, 1 /* + */);
    TEST_ITEM;
    /* S -> ·T, $/+ */
    item = Item_new(2 /* S -> T */, 0 /* ·T */, 2, 9 /* $ */, 1 /* + */);
    TEST_ITEM;
    /* T -> ·TF, $/+/a/b */
    item = Item_new(3 /* T -> TF */, 0 /* ·TF */, 4, 9 /* $ */, 1 /* + */,
        3 /* a */, 4 /* b */);
    TEST_ITEM;
    /* T -> ·F, $/+/a/b */
    item = Item_new(4 /* T -> F */, 0 /* ·F */, 4, 9 /* $ */, 1 /* + */,
        3 /* a */, 4 /* b */);
    TEST_ITEM;
    /* F -> ·F*, $/+/a/b/ * */
    item = Item_new(5 /* F -> F* */, 0 /* ·F* */, 5, 9 /* $ */, 1 /* + */,
        2 /* * */, 3 /* a */, 4 /* b */);
    TEST_ITEM;
    /* F -> ·a, $/+/a/b/ * */
    item = Item_new(6 /* F -> a */, 0 /* ·a */, 5, 9 /* $ */, 1 /* + */,
        2 /* * */, 3 /* a */, 4 /* b */);
    TEST_ITEM;
    /* F -> ·b, $/+/a/b/ * */
    item = Item_new(7 /* F -> b */, 0 /* ·b */, 5, 9 /* $ */, 1 /* + */,
        2 /* * */, 3 /* a */, 4 /* b */);
    TEST_ITEM;

    set_out(&s);
    Grammar_out(g);
    free(g);
}
