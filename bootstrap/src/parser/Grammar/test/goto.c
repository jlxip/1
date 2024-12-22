#include "../easy.h"
#include "../src/Grammar.h"
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

void test_lalr_goto(void) {
    Grammar *g;
    Item item;
    set s, s2;

    g = grammar(tokens, nts, test, "S");
    Grammar_augment(g);
    Grammar_compute_firsts(g);

    /*
        I0 = closure(I)
        I3 = closure(goto(I0, c))
    */
    item = Item_new(3 /* S' -> S */, 0 /* ·S */, 1, 6 /* $ */);
    s = Grammar_closure(g, &item, 0);
    destroy_item(&item);
    s2 = Grammar_goto(g, s, 1, 0);
    set_out(&s);
    s = s2;
    s2 = NULL;

    TEST_NUM(3);
    /* C -> c·C, c/d */
    item = Item_new(1 /* C -> cC */, 1 /* c·C */, 2, 1, 2);
    TEST_ITEM;
    /* C -> ·cC, c/d */
    item = Item_new(1 /* C -> cC */, 0 /* ·cC */, 2, 1, 2);
    TEST_ITEM;
    /* C -> ·d, c/d */
    item = Item_new(2 /* C -> d */, 0 /* ·d */, 2, 1, 2);
    TEST_ITEM;

    /*
        I8 = closure(goto(I3, C))
        I8: C -> cC·, c/d
    */
    s2 = s;
    s = Grammar_goto(g, s2, 5, 0);
    set_out(&s2);
    TEST_NUM(1);
    /* C -> cC·, c/d */
    item = Item_new(1 /* C -> cC */, END_OF_PRODUCTION /* cC· */, 2, 1, 2);
    TEST_ITEM;
    set_out(&s);

    Grammar_out(g);
    free(g);
}
