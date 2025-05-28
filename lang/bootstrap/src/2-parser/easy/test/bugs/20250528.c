#include "helpers.h"

static const char *tokens_20250528[5] = {"a", "b", "c", "d", NULL};
static const char *nts_20250528[3] = {"S", "A", NULL};
static const char *grammar_20250528 = "S -> a A c\n"
                                      "S -> a A d\n"
                                      "A -> b\n";

#define MAKE_THIS MAKE_GENERIC(20250528)

static void test_bug(void) {
    Grammar *g;
    TokenData stream[4];

    g = MAKE_THIS;
    grammar_compile(g);

    /* Case 1 */
    stream[0].sym = 1;
    stream[1].sym = 2;
    stream[2].sym = 3;
    stream[0].data = stream[1].data = stream[2].data = stream[3].data = 0;
    stream[3].sym = 0;
    grammar_parse(g, stream);

    /* Case 2 */
    stream[2].sym = 4;
    grammar_parse(g, stream);

    grammar_out(g);
}

static void test_easy(void) {
    Grammar *g;
    const Production *prod;

    g = MAKE_THIS;
    Grammar_augment(g); /* !!! */

    assert(g->ntok == 5);
    assert(g->nsym == 8);
    assert(buffer_num(g->g) == 4);
    /* [0] E -> S */
    prod = buffer_get(g->g, 0, Production);
    assert(prod->lhs == 5);
    assert(buffer_num(prod->rhs) == 1);
    assert(*buffer_get(prod->rhs, 0, symbol) == 6);
    /* [1] S -> a A c */
    prod = buffer_get(g->g, 1, Production);
    assert(prod->lhs == 6);
    assert(buffer_num(prod->rhs) == 3);
    assert(*buffer_get(prod->rhs, 0, symbol) == 1);
    assert(*buffer_get(prod->rhs, 1, symbol) == 7);
    assert(*buffer_get(prod->rhs, 2, symbol) == 3);
    /* [2] S -> a A d */
    prod = buffer_get(g->g, 2, Production);
    assert(prod->lhs == 6);
    assert(buffer_num(prod->rhs) == 3);
    assert(*buffer_get(prod->rhs, 0, symbol) == 1);
    assert(*buffer_get(prod->rhs, 1, symbol) == 7);
    assert(*buffer_get(prod->rhs, 2, symbol) == 4);
    /* [3] A -> b */
    prod = buffer_get(g->g, 3, Production);
    assert(prod->lhs == 7);
    assert(buffer_num(prod->rhs) == 1);
    assert(*buffer_get(prod->rhs, 0, symbol) == 2);
}

static void test_first(void) {
    Grammar *g;
    size_t x;
    set s;

    g = MAKE_THIS;
    Grammar_augment(g); /* !!! */

    /* FIRST(E) = { a } */
    s = FIRST(g, 5);
    TEST_NUM(1);
    TEST_HAS(1);
    /* FIRST(S) = { a } */
    s = FIRST(g, 6);
    TEST_NUM(1);
    TEST_HAS(1);
    /* FIRST(A) = { b } */
    s = FIRST(g, 7);
    TEST_NUM(1);
    TEST_HAS(2);

    Grammar_out(g);
    free(g);
}

static void test_collection(void) {
    Grammar *g;
    set s = NULL;
    Item item;

    g = MAKE_THIS;
    Grammar_augment(g);
    Grammar_compute_collection(g);

    /*
        I0: E -> ·S, $
            S -> ·aAc, $
            S -> ·aAd, $
    */
    set_new_Item(&s);
    item = Item_new(0 /* E -> S */, 0 /* ·S */, 1, 8 /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(1 /* S -> aAc */, 0 /* ·aAc */, 1, 8 /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(2 /* S -> aAd */, 0 /* ·aAd */, 1, 8 /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    CHECK_STATE;
    set_out(&s);

    /*
        I1: E -> S·, $
    */
    set_new_Item(&s);
    item = Item_new(0 /* E -> S */, END_OF_PRODUCTION /* S· */, 1, 8 /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    CHECK_STATE;
    set_out(&s);

    /*
        I2: S -> a·Ac, $
            S -> a·Ad, $
            A -> ·b, c/d
    */
    set_new_Item(&s);
    item = Item_new(1 /* S -> aAc */, 1 /* a·Ac */, 1, 8 /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(2 /* S -> aAd */, 1 /* a·Ad */, 1, 8 /* $ */);
    set_add(s, &item);
    destroy_item(&item);
    item = Item_new(3 /* A -> b */, 0 /* ·b */, 2, 3, 4);
    set_add(s, &item);
    destroy_item(&item);
    CHECK_STATE; /* !!! BUG MANIFESTS HERE !!! */
    set_out(&s);

    Grammar_out(g);
    free(g);
}

static void test_goto(void) {
    Grammar *g;
    Item item;
    set s, s2;

    g = MAKE_THIS;
    Grammar_augment(g);

    /*
        I0 = closure(I)
        I2 = closure(goto(I0, a))
    */
    item = Item_new(0 /* E -> S */, 0 /* ·S */, 1, 8 /* $ */);
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

    /*print_set_of_items(s);*/

    TEST_NUM(3); /* !!! BUG MANIFESTS HERE !!! */
    /* S -> a·Ac, $ */
    item = Item_new(1 /* S -> aAc */, 1 /* a·Ac */, 1, 8 /* $ */);
    TEST_ITEM;
    /* S -> a·Ad, $ */
    item = Item_new(2 /* S -> aAd */, 1 /* a·Ad */, 1, 8 /* $ */);
    TEST_ITEM;
    /* A -> ·d, c/d */
    item = Item_new(3 /* A -> d */, 0 /* ·d */, 2, 3, 4);
    TEST_ITEM;

    set_out(&s);
    Grammar_out(g);
    free(g);
}

void test_20250528() {
    /* These did work */
    test_easy();
    test_first();

    /* These did not */
    test_goto();
    test_collection();
    /*test_bug(); <-- Commented because of verbosity in DEBUG mode */
    (void)test_bug;
}
