#include "../easy.h"
#include "../src/internal.h"
#include <common.h>

#define CHECK_STATE                                                            \
    do {                                                                       \
        size_t found = 0, i;                                                   \
        for (i = 0; i < buffer_num(g->collection); ++i) {                      \
            if (equal_set(buffer_get(g->collection, i, set), &s)) {            \
                found = 1;                                                     \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        assert(found);                                                         \
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

void test_lalr_collection(void) {
    Grammar *g;
    set s = NULL;
    Item item;

    g = grammar(tokens, nts, test, "S");
    Grammar_compute_collection(g);

    /*
        I0: S' -> ·S, $
            S  -> ·CC, $
            C  -> ·cC, c/d
            C  -> ·d, c/d
    */
    set_new_Item(&s);
    item = Item_new(0 /* S' -> S */, 0 /* ·S */, 1, 6 /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(1 /* S -> CC */, 0 /* ·CC */, 1, 6 /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(2 /* C -> cC */, 0 /* ·cC */, 2, 1, 2);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(3 /* C -> d */, 0 /* ·d */, 2, 1, 2);
    set_add(s, &item);
    destroy_item(&item);
    CHECK_STATE;
    set_out(&s);

    /*
        I1: S' -> S·, $
    */
    set_new_Item(&s);
    item = Item_new(0 /* S' -> S */, END_OF_PRODUCTION /* S· */, 1, 6 /* $ */);
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
    item = Item_new(1 /* S -> CC */, 1 /* C·C */, 1, 6 /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(2 /* C -> cC */, 0 /* ·cC */, 1, 6 /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(3 /* C -> d */, 0 /* ·d */, 1, 6 /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    CHECK_STATE;
    set_out(&s);

    /*
        I36: C -> c·C, c/d/$
             C -> ·cC, c/d/$
             C -> ·d, c/d/$
    */
    set_new_Item(&s);
    item = Item_new(2 /* C -> cC */, 1 /* c·C */, 3, 1, 2, 6 /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(2 /* C -> cC */, 0 /* ·cC */, 3, 1, 2, 6 /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(3 /* C -> d */, 0 /* ·d */, 3, 1, 2, 6 /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    CHECK_STATE;
    set_out(&s);

    /*
        I47: C -> d·, c/d/$
    */
    set_new_Item(&s);
    item = Item_new(
        2 /* C -> d */, END_OF_PRODUCTION /* d· */, 3, 1, 2, 6 /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    CHECK_STATE;
    set_out(&s);

    /*
        I5: S -> CC·, $
    */
    set_new_Item(&s);
    item = Item_new(1 /* S -> CC */, END_OF_PRODUCTION /* CC· */, 1, 6 /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    CHECK_STATE;
    set_out(&s);

    /*
        I89: C -> cC·, c/d/$
    */
    set_new_Item(&s);
    item = Item_new(
        2 /* C -> cC */, END_OF_PRODUCTION /* cC· */, 3, 1, 2, 6 /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    CHECK_STATE;
    set_out(&s);

    Grammar_out(g);
    free(g);
}
