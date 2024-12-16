#include "../Grammar.h"

static const size_t EEPSILON = EPSILON;

/* LR(1) item closure: [A -> alpha · B beta, a] */
/* This expands only once */
static set Grammar_closure_once(
    const Grammar *g, const Item *item, size_t core) {
    set ret = NULL; /* set<Item> */
    const Production *prod;
    size_t sym;
    size_t i;
    buffer beta = NULL;

    if (!core)
        set_new_Item(&ret);
    else
        set_new_Item_core(&ret);
    set_add(ret, item);

    /* If dot is at the end of item, no additional closure */
    if (item->dot == END_OF_PRODUCTION)
        return ret;

    /* Get symbol next to the dot */
    prod = buffer_get(g->g, item->prod, Production);
    sym = *buffer_get(prod->rhs, item->dot, size_t);

    /* If it's a terminal, we're done, return item */
    if (IS_TERMINAL(g, sym))
        return ret;

    /* beta = prod[dot+1:] */
    buffer_new(&beta, sizeof(size_t));
    for (i = item->dot + 1; i < buffer_num(prod->rhs); ++i)
        buffer_push(beta, buffer_get(prod->rhs, i, void));

    /* It's a non-terminal, expand */
    for (i = 0; i < buffer_num(g->g); ++i) {
        Item *new;
        const Production *p = buffer_get(g->g, i, Production);
        if (p->lhs != sym)
            continue; /* not interested */

        new = malloc(sizeof(Item));
        new->prod = i;
        new->dot = 0;

        /* Now compute second component, FIRST(beta a) */
        new->look = Grammar_first_many(g, beta);

        if (set_empty(new->look)) {
            /* No firsts; so, epsilon. Add a */
            set_join(new->look, item->look);
        } else if (set_has(new->look, &EEPSILON)) {
            /* Epsilon in ret: remove it and add a */
            set_remove(new->look, &EEPSILON);
            set_join(new->look, item->look);
        }

        set_add_move(ret, new);
    }

    buffer_out(&beta);
    return ret;
}

/* Acutal iterative LR(1) item closure */
set Grammar_closure(const Grammar *g, const Item *item, size_t core) {
    set ret = NULL;
    size_t prev = 0;
    size_t now;

    /* Expand for the first time */
    ret = Grammar_closure_once(g, item, core);
    if (!ret)
        return NULL;
    if (set_num(ret) == 1)
        return ret; /* If there were something to expand it would've already
                       happened */

    /* Just keep expanding until there's no changes */
    now = set_num(ret);
    while (now > prev) {
        set to_add = NULL; /* set<Item> */
        set_iterator it;

        if (!core)
            set_new_Item(&to_add);
        else
            set_new_Item_core(&to_add);
        it = set_it_begin(ret);
        while (!set_it_finished(&it)) {
            const Item *this = set_it_get(&it, Item);
            set aux = Grammar_closure_once(g, this, core); /* set<Item> */
            if (!aux) {
                /* Nothing to do here */
                set_it_next(&it);
                continue;
            }
            if (set_num(aux) == 1) {
                /* No new expansions */
                set_out(&aux);
                set_it_next(&it);
                continue;
            }

            /* Save the new ones */
            set_join_move(to_add, aux);
            set_it_next(&it);
        }

        /* Now that we're done iterating, add the new ones */
        set_join_move(ret, to_add);
        prev = now;
        now = set_num(ret);
    }

    return ret;
}
