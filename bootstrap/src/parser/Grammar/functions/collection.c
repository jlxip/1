#include "../Grammar.h"

const size_t ZERO = 0;

static void build_I0(Grammar *g, map seen) {
    /* Everything starts with the augmented start */
    Item start;
    set I0;

    start = Item_new(g->ntok /* Augmented start symbol */,
        0 /* Beginning of production */, 1, g->nsym /* $ */);

    I0 = Grammar_closure(g, &start, 1);
    destroy_item(&start);
    map_add(seen, &I0, &ZERO);
    buffer_push(g->collection, &I0);
}

static size_t join_looks(set a, set b) {
    size_t changed = 0;

    set_iterator it = set_it_begin(b);
    while (!set_it_finished(&it)) {
        Item *ib = set_it_get(&it, Item);
        Item *ia = set_get(a, ib, Item);
        size_t before;

        before = set_num(ia->look);
        set_join(ia->look, ib->look);
        changed |= set_num(ia->look) > before;
        set_it_next(&it);
    }

    return changed;
}

static void recursive_goto(Grammar *g, map seen) {
    size_t changed = 0;
    do {
        size_t idx;
        changed = 0;
        for (idx = 0; idx < buffer_num(g->collection); ++idx) {
            /* Note that collection may increase with each iteration */
            set parent; /* Parent state: set<Item> */
            size_t sym;

            parent = *buffer_get(g->collection, idx, set);

            /* Try all symbols and see what sticks */
            for (sym = 1; sym < g->nsym; ++sym) {
                set child; /* Child state: set<Item> */
                child = Grammar_goto(g, parent, sym, 1);
                if (set_empty(child)) {
                    /* Nothing to do with this symbol */
                    set_out(&child);
                    continue;
                }

                /* Already seen? */
                if (map_has(seen, &child)) {
                    /* Yes, join lookahead symbols */
                    size_t prev = *map_get(seen, &child, size_t);
                    set previous;

                    previous = *buffer_get(g->collection, prev, set);
                    if (join_looks(previous, child) && prev <= idx) {
                        /*
                            The idea here is that we only have to iterate again
                            if some previous state has changed
                        */
                        changed = 1;
                    }
                    set_out(&child);
                } else {
                    /* No, new state */
                    size_t prev = buffer_num(g->collection);
                    map_add(seen, &child, &prev);
                    buffer_push(g->collection, &child);
                }
            }
        }
    } while (changed);
}

static void fix_collection(Grammar *g) {
    size_t idx;
    for (idx = 0; idx < buffer_num(g->collection); ++idx) {
        set old;        /* set<Item (core)> */
        set new = NULL; /* set<Item> */

        old = *buffer_get(g->collection, idx, set);
        set_new_Item(&new);
        set_join_move(new, old);
        buffer_set(g->collection, idx, &new);
    }
}

void Grammar_compute_collection(Grammar *g) {
    map seen = NULL; /* map<set<Item (core)>, size_t (index in collection)> */

    if (!g->firsts)
        throw("tried to call compute_collection() without computing firsts");

    map_new(&seen, hash_set, equal_set, copy_set, destroy_set, sizeof(size_t),
        hash_size_t, equal_size_t, copy_size_t, destroy_size_t);
    buffer_new(&g->collection, sizeof(buffer));
    /*buffer_new(&g->gotos_forwards, sizeof(set));
    buffer_new(&g->gotos_backwards, sizeof(Goto));
    buffer_push(g->gotos_backwards, &nullgoto);*/ /* Nothing goes to I0 */

    /* Get the first state */
    build_I0(g, seen);

    /* And start performing GOTOs to other states */
    recursive_goto(g, seen);
    map_out(&seen);

    /*
        g->collection has been built as buffer<set<Item (core)>> in order to
        join the lookahead symbols of the cores. However, we actually want
        buffer<set<Item>>
    */
    fix_collection(g);
}
