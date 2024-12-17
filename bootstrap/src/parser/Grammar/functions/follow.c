#include "../Grammar.h"

static const bool EFALSE = false;
static const bool ETRUE = true;
static const symbol EEPSILON = EPSILON;

/* --- FOLLOW --- */

/* Checks if sym -*> epsilon, saves the result */
static void compute_derives_to_epsilon(Grammar *g, symbol sym) {
    bool result = false;
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
            result = true;
            break;
        }
    }

    map_add(g->epsilons, &sym, &result);
}

static void rec_compute_follow(Grammar *g, symbol sym, map cache, set stack) {
    size_t iprod;
    set *ret = NULL; /* set<symbol> */

    if (map_has(cache, &sym) || set_has(stack, &sym))
        return;

    set_add(stack, &sym);
    ret = calloc(1, sizeof(set));
    set_new_symbol(ret);

    /* Start symbol includes $ */
    if (sym == g->start)
        set_add(*ret, &g->nsym); /* Remember, NSYM = $ */

    /* Check productions */
    for (iprod = 0; iprod < buffer_num(g->g); ++iprod) {
        const Production *x = buffer_get(g->g, iprod, Production);
        bool found = false;
        size_t irhs;

        /* Find sym in rhs */
        for (irhs = 0; irhs < buffer_num(x->rhs); ++irhs) {
            symbol *e = buffer_get(x->rhs, irhs, symbol);
            if (!found && *e == sym) {
                found = true;
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
                set_join(*ret, *parent);
        } else {
            /* There is! */
            symbol follow = *buffer_get(x->rhs, irhs + 1, symbol);
            /*
                There's something to the right: A -> B X
                FOLLOW(B) must include FIRST(X) - { epsilon }
            */
            set_join(*ret, *map_get(g->firsts, &follow, set));
            set_remove_if_there(*ret, &EEPSILON);

            /*
                Additionally, if X -*> epsilon, then
                FOLLOW(B) must include FOLLOW(X)
            */
            if (*map_get(g->epsilons, &follow, bool)) {
                /* FOLLOW(B) += FOLLOW(X) */
                const set *followx;
                rec_compute_follow(g, follow, cache, stack);
                followx = map_get(cache, &follow, set);
                if (followx) {
                    set_join(*ret, *followx);
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
    map_add_movev(cache, &sym, ret);
    set_remove(stack, &sym);
}

void Grammar_compute_follows(Grammar *g) {
    set stack = NULL; /* set<symbol> */
    size_t i;

    if (map_empty(g->firsts))
        throw("tried to call compute_follows() without firsts");

    map_new_symbol(
        &g->follows, sizeof(set), hash_set, equal_set, copy_set, destroy_set);
    set_new_symbol(&stack);

    /* First compute, for all NTs, if X -*> epsilon */
    map_new_symbol(&g->epsilons, sizeof(bool), hash_bool, equal_bool, copy_bool,
        destroy_bool);
    map_add(g->epsilons, &EEPSILON, &ETRUE); /* epsilon -*> epsilon */
    for (i = 1; i < g->ntok; ++i)
        map_add(g->epsilons, &i, &EFALSE); /* token !-*> epsilon */
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
