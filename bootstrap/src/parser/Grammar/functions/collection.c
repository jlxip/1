#include "../Grammar.h"

static void build_I0(Grammar *g) {
    /* Everything starts with the augmented start */
    Item start;
    set I0;

    start = Item_new(g->ntok /* Augmented start symbol */,
        0 /* Beginning of production */, 1, g->nsym /* $ */);

    I0 = Grammar_closure(g, &start);
    destroy_item(&start);
    buffer_push(g->collection, &I0);
}

static void recursive_goto(Grammar *g) {
    set seen = NULL; /* Already seen states: set<set<Item>> */
    size_t idx;

    set_new_set(&seen);
    for (idx = 0; idx < buffer_num(g->collection); ++idx) {
        /* Note that collection may increase with each iteration */
        set parent; /* Parent state: set<Item> */
        size_t sym;

        parent = *buffer_get(g->collection, idx, set);

        /* Try all symbols and see what sticks */
        for (sym = 1; sym < g->nsym; ++sym) {
            set child; /* Child state: set<Item> */
            child = Grammar_goto(g, parent, sym);
            if (set_empty(child)) {
                /* Nothing to do with this symbol */
                set_out(&child);
                continue;
            }

            /* Already seen? */
            if (set_has(seen, &child)) {
                /* Yep */
                set_out(&child);
                continue;
            }

            /* Got a new state */
            buffer_push(g->collection, &child);
            set_add(seen, &child);
        }
    }

    set_out(&seen);
}

void Grammar_compute_collection(Grammar *g) {
    if (!g->firsts)
        throw("tried to call compute_collection() without computing firsts");

    buffer_new(&g->collection, sizeof(set));
    /*buffer_new(&g->gotos_forwards, sizeof(set));
    buffer_new(&g->gotos_backwards, sizeof(Goto));
    buffer_push(g->gotos_backwards, &nullgoto);*/ /* Nothing goes to I0 */

    /* Get the first state */
    build_I0(g);

    /* And start performing GOTOs to other states */
    recursive_goto(g);
}
