#include "../Grammar.h"

static const size_t EZERO = 0;
static const size_t EONE = 1;
static const size_t EEPSILON = EPSILON;

/* --- FOLLOW --- */

/* Checks if sym -*> epsilon, saves the result */
static void compute_derives_to_epsilon(Grammar *g, size_t sym) {
    size_t result = 0;
    size_t iprod;

    if (map_has(g->epsilons, &sym))
        return;

    /* Check productions */
    for (iprod = 0; iprod < buffer_num(g->g); ++iprod) {
        const Production *x = buffer_get(g->g, iprod, Production);

        /* Find sym in lhs */
        if (x->lhs != sym)
            continue; /* not interested */

        /* rhs is empty if sym -*> epsilon */
        if (buffer_empty(x->rhs)) {
            /* Nice */
            result = 1;
            break;
        }
    }

    map_add(g->epsilons, &sym, &result);
}

static void rec_compute_follow(Grammar *g, size_t sym, map cache, set stack) {
    size_t iprod;
    set ret = NULL; /* set<size_t> */

    if (map_has(cache, &sym) || set_has(stack, &sym))
        return;

    set_add(stack, &sym);
    set_new_size_t(&ret);

    /* Start symbol includes $ */
    if (sym == g->start)
        set_add(ret, &g->nsym); /* Remember, NSYM = $ */

    /* Check productions */
    for (iprod = 0; iprod < buffer_num(g->g); ++iprod) {
        const Production *x = buffer_get(g->g, iprod, Production);
        size_t found = 0;
        size_t irhs;

        /* Find sym in rhs */
        for (irhs = 0; irhs < buffer_num(x->rhs); ++irhs) {
            size_t *e = buffer_get(x->rhs, irhs, size_t);
            if (!found && *e == sym) {
                found = 1;
                break;
            }
        }

        if (!found)
            continue; /* Not interested in this production */

        /* We got sym, is there something to the right? */
        if (irhs == buffer_num(x->rhs) - 1) {
            /*
                Nope. This is the end of the production.
                We're in this case:
                A -> B ===> FOLLOW(B) = FOLLOW(A)
            */
            const set *parent;
            rec_compute_follow(g, x->lhs, cache, stack);
            parent = map_get(cache, &x->lhs, set);
            if (parent)
                set_join(ret, *parent);
        } else {
            /* There is! */
            size_t follow = *buffer_get(x->rhs, irhs + 1, size_t);
            /*
                There's something to the right: A -> B X
                FOLLOW(B) must include FIRST(X) - { epsilon }
            */
            set_join(ret, *map_get(g->firsts, &follow, set));
            set_remove_if_there(ret, &EEPSILON);

            /*
                Additionally, if X -*> epsilon, then
                FOLLOW(B) must include FOLLOW(X)
            */
            if (*map_get(g->epsilons, &follow, size_t)) {
                /* FOLLOW(B) += FOLLOW(X) */
                const set *followx;
                rec_compute_follow(g, follow, cache, stack);
                followx = map_get(cache, &follow, set);
                if (followx) {
                    set_join(ret, *followx);
                } else {
                    /*
                        Called to rec_compute_follow but got nothing.
                        This is fine, probably a recursive scenario, such as:
                            FOLLOW(B) += FOLLOW(B)
                        Just do nothing.
                    */
                }
            }
        }
    }

    /* Save this result */
    map_add_move(cache, &sym, &ret);
    set_remove(stack, &sym);
}

void Grammar_compute_follows(Grammar *g) {
    set stack = NULL; /* set<size_t> */
    size_t i;

    if (map_empty(g->firsts))
        throw("tried to call compute_follows() without firsts");

    map_new_size_t(
        &g->follows, sizeof(set), hash_set, equal_set, copy_set, destroy_set);
    set_new_size_t(&stack);

    /* First compute, for all NTs, if X -*> epsilon */
    map_new_size_t(&g->epsilons, sizeof(size_t), hash_size_t, equal_size_t,
        copy_size_t, destroy_size_t);
    map_add(g->epsilons, &EZERO, &EONE); /* epsilon -*> epsilon */
    for (i = 1; i < g->ntok; ++i)
        map_add(g->epsilons, &i, &EZERO); /* token !-*> epsilon */
    for (i = g->ntok + 1; i < g->nsym; ++i)
        compute_derives_to_epsilon(g, i); /* compute the rest */

    /* Now compute FOLLOW(A) for all NTs */
    /* Grammar is augmented so ntok is the start and is included */
    for (i = g->ntok; i < g->nsym; ++i) {
        rec_compute_follow(g, i, g->follows, stack);
        assert(set_empty(stack));
    }

    set_out(&stack);
}
