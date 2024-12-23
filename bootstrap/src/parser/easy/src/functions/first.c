#include "../internal.h"

static const symbol EEPSILON = EPSILON;

/* --- FIRST --- */

static void rec_compute_first(Grammar *g, symbol sym, map cache, set stack) {
    size_t iprod;
    set *ret = NULL; /* set<symbol> */

    if (map_has(cache, &sym) || set_has(stack, &sym))
        return;

    set_add(stack, &sym);

    ret = calloc(1, sizeof(set));
    set_new_symbol(ret);

    if (IS_TERMINAL(g, sym)) {
        set_add(*ret, &sym);
        goto out;
    }

    /* Check productions */
    for (iprod = 0; iprod < buffer_num(g->g); ++iprod) {
        const Production *x = buffer_get(g->g, iprod, Production);
        bool epsilon = true;
        size_t irhs;

        /* Find sym in lhs */
        if (x->lhs != sym)
            continue; /* not interested */

        /* Go through rhs */
        for (irhs = 0; irhs < buffer_num(x->rhs); ++irhs) {
            const symbol *e = buffer_get(x->rhs, irhs, symbol);
            const set *sub;
            rec_compute_first(g, *e, cache, stack);
            sub = map_get(cache, e, set);

            if (sub == NULL) {
                /* Called myself, skip */
                continue;
            }

            set_join(*ret, *sub);
            set_remove_if_there(*ret, &EEPSILON);
            if (!set_empty(*sub) && !set_has(*sub, &EEPSILON)) {
                epsilon = false;
                break;
            }
        }

        if (epsilon)
            set_add(*ret, &EEPSILON);
    }

out:
    /* Save this result */
    map_add_movev(cache, &sym, ret);
    set_remove(stack, &sym);
}

set FIRST(Grammar *g, symbol sym) {
    set stack = NULL; /* set<symbol> */

    if (!g->augmented)
        Grammar_augment(g);

    if (g->firsts == NULL)
        map_new_symbol(&g->firsts, sizeof(set), hash_set, equal_set, copy_set,
            destroy_set);

    if (sym == 0)
        throw("FIRST(epsilon) does not exist");

    if (map_has(g->firsts, &sym))
        return *map_get(g->firsts, &sym, set);

    set_new_symbol(&stack);
    rec_compute_first(g, sym, g->firsts, stack);
    assert(set_empty(stack));
    set_out(&stack);

    assert(map_has(g->firsts, &sym));
    return *map_get(g->firsts, &sym, set);
}

/* Computes FIRST(alpha), such as FIRST(A B C) */
set FIRST_MANY(Grammar *g, const buffer syms) {
    set ret = NULL; /* set<symbol> */
    size_t i;

    /*
        FIRST(A B) =
            FIRST(A), if epsilon not in A
            FIRST(A) - { epsilon } + FIRST(B), otherwise
    */

    set_new_symbol(&ret);
    for (i = 0; i < buffer_num(syms); ++i) {
        symbol sym = *buffer_get(syms, i, symbol);
        set other = FIRST(g, sym);
        set_join(ret, other);

        if (!set_has(ret, &EEPSILON))
            break;

        if (i != buffer_num(syms) - 1)
            set_remove(ret, &EEPSILON);
    }

    return ret;
}
