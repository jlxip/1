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

void test_lalr_goto(void) {
    Grammar *g;
    Item item;
    set s, s2;

    g = grammar(tokens, nts, test, "S");

    /*
        I0 = closure(I)
        I3 = closure(goto(I0, c))
    */
    item = Item_new(0 /* S' -> S */, 0 /* ·S */, 1, 6 /* $ */);
    s = CLOSURE(g, &item);
    destroy_item(&item);
    s2 = GOTO(g, s, 1);
    set_out(&s);
    s = s2;
    s2 = NULL;
    set_new_Item(&s2);
    set_migrate(s2, s);
    s = s2;
    s2 = NULL;

    TEST_NUM(3);
    /* C -> c·C, c/d */
    item = Item_new(2 /* C -> cC */, 1 /* c·C */, 2, 1, 2);
    TEST_ITEM;
    /* C -> ·cC, c/d */
    item = Item_new(2 /* C -> cC */, 0 /* ·cC */, 2, 1, 2);
    TEST_ITEM;
    /* C -> ·d, c/d */
    item = Item_new(3 /* C -> d */, 0 /* ·d */, 2, 1, 2);
    TEST_ITEM;

    /*
        I8 = closure(goto(I3, C))
        I8: C -> cC·, c/d
    */
    s2 = s;
    s = GOTO(g, s2, 5);
    set_out(&s2);
    s2 = NULL;
    set_new_Item(&s2);
    set_migrate(s2, s);
    s = s2;
    s2 = NULL;
    TEST_NUM(1);
    /* C -> cC·, c/d */
    item = Item_new(2 /* C -> cC */, END_OF_PRODUCTION /* cC· */, 2, 1, 2);
    TEST_ITEM;
    set_out(&s);
    Grammar_out(g);
    free(g);

    /* The other grammar
       I1 = goto(I0, 'a')
       I1: F -> a·, $/+/a/b/ *
    */
    g = grammar(tokens2, nts2, test2, "S");
    item = Item_new(0 /* S' -> S * */, 0 /* ·S */, 1, 9 /* $ */);
    s = CLOSURE(g, &item);
    destroy_item(&item);
    s2 = GOTO(g, s, 3);
    set_out(&s);
    s = s2;
    s2 = NULL;
    set_new_Item(&s2);
    set_migrate(s2, s);
    s = s2;
    s2 = NULL;

    TEST_NUM(1);
    /* F -> a·, $/+/a/b/ * */
    item = Item_new(6 /* F -> a */, END_OF_PRODUCTION /* a· */, 5, 9 /* $ */,
        1 /* + */, 2 /* * */, 3 /* a */, 4 /* b */);
    TEST_ITEM;
    set_out(&s);
    Grammar_out(g);
    free(g);
}
