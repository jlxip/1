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

void test_lalr_closure(void) {
    Grammar *g;
    set s;
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
    s = CLOSURE(g, &item, 0);
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
    s = CLOSURE(g, &item, 0);
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
}
