#include "../Grammar.h"

static const size_t EEPSILON = EPSILON;

/* --- FIRST --- */

static void rec_compute_first(Grammar *g, size_t sym, map cache, set stack) {
    size_t iprod;
    set *ret = NULL; /* set<size_t> */

    if (map_has(cache, &sym) || set_has(stack, &sym))
        return;

    set_add(stack, &sym);

    /* If not in cache, surely not terminal */
    ret = calloc(1, sizeof(set));
    set_new_size_t(ret);

    /* Check productions */
    for (iprod = 0; iprod < buffer_num(g->g); ++iprod) {
        const Production *x = buffer_get(g->g, iprod, Production);
        size_t epsilon = 1;
        size_t irhs;

        /* Find sym in lhs */
        if (x->lhs != sym)
            continue; /* not interested */

        /* Go through rhs */
        for (irhs = 0; irhs < buffer_num(x->rhs); ++irhs) {
            size_t *e = buffer_get(x->rhs, irhs, size_t);
            const set *sub;
            rec_compute_first(g, *e, cache, stack);
            sub = map_get(cache, e, set);

            if (sub == NULL) {
                /* Called myself, skip */
                continue;
            } else if (set_has(*sub, &EEPSILON)) {
                /* Remove epsilon from sub, join, and keep going */
                set_join(*ret, *sub);
                set_remove(*ret, &EEPSILON);
            } else {
                /* Join and we're done */
                epsilon = 0;
                set_join(*ret, *sub);
                break;
            }
        }

        if (epsilon)
            set_add(*ret, &EEPSILON);
    }

    /* Save this result */
    map_add_movev(cache, &sym, ret);
    set_remove(stack, &sym);
}

void Grammar_compute_firsts(Grammar *g) {
    set stack = NULL; /* set<size_t> */
    size_t i;

    if (!g->augmented)
        throw("tried to call compute_first() on non-augmented grammar");

    map_new_size_t(
        &g->firsts, sizeof(set), hash_set, equal_set, copy_set, destroy_set);
    set_new_size_t(&stack);

    for (i = 1; i < g->ntok; ++i) {
        set *s = calloc(1, sizeof(set));
        set_new_size_t(s);
        set_add(*s, &i);
        map_add_movev(g->firsts, &i, s);
    }

    /* It's augmented so ntok is the start and is included */
    for (i = g->ntok; i < g->nsym; ++i) {
        rec_compute_first(g, i, g->firsts, stack);
        assert(set_empty(stack));
    }

    set_out(&stack);
}

/* Computes FIRST(alpha), such as FIRST(A B C) */
set Grammar_first_many(const Grammar *g, const buffer syms) {
    set ret = NULL; /* set<size_t> */
    size_t i;

    if (!g->firsts)
        throw("tried to call first_many() without computing firsts");

    /*
        FIRST(A B) =
            FIRST(A), if epsilon not in A
            FIRST(A) - { epsilon } + FIRST(B), otherwise
    */

    set_new_size_t(&ret);
    for (i = 0; i < buffer_num(syms); ++i) {
        size_t sym = *buffer_get(syms, i, size_t);
        const set *other = map_get(g->firsts, &sym, set);
        set_join(ret, *other);

        if (!set_has(ret, &EEPSILON))
            break;

        set_remove(ret, &EEPSILON);
    }

    return ret;
}
