#include "../Grammar.h"

static const size_t EEPSILON = EPSILON;

/* LR(1) item closure: [A -> alpha · B beta, a] */
/* This expands only once */
static set Grammar_closure_once(const Grammar *g, const Item *item) {
    set ret = NULL; /* set<Item> */
    const Production *prod;
    size_t sym;
    size_t i;
    Item copy;
    buffer beta = NULL;

    /* If dot is at the end of item, no closure */
    if (item->dot == END_OF_PRODUCTION)
        return ret;

    set_new(&ret, sizeof(Item));

    /* Add the item */
    copy = *item;
    copy.look = set_copy(copy.look);
    set_add(ret, &copy);

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
        Item new;
        const Production *p = buffer_get(g->g, i, Production);
        if (p->lhs != sym)
            continue; /* not interested */

        new.prod = i;
        new.dot = 0;
        /*new.look = NULL;*/

        /* Now compute second component, FIRST(beta a) */
        new.look = Grammar_first_many(g, beta);

        if (set_empty(new.look)) {
            /* No firsts; so, epsilon. Add a */
            set_join(new.look, item->look);
        } else if (set_has(new.look, &EEPSILON)) {
            /* Epsilon in ret: remove it and add a */
            set_remove(new.look, &EEPSILON);
            set_join(new.look, item->look);
        }

        set_add(ret, &new);
    }

    buffer_out(&beta);
    return ret;
}

/* This has to be done manually since set sucks and compares shallow */
/* I believe this can't be helped in ANSI C */
static size_t hasitem(const set *s, const Item *item) {
    size_t i, j;
    for (i = 0; i < set_num(*s); ++i) {
        const Item *other = buffer_get(*s, i, Item);
        if (item->prod != other->prod)
            continue;
        if (item->dot != other->dot)
            continue;
        /* Compare second component */
        if (set_num(item->look) != set_num(other->look))
            continue;
        for (j = 0; j < set_num(item->look); ++j) {
            size_t sym = *buffer_get(item->look, j, size_t);
            if (!set_has(other->look, &sym))
                goto nope;
        }

        return 1; /* Success! */

    nope:
        continue;
    }

    return 0;
}

/* Acutal iterative LR(1) item closure */
set Grammar_closure(const Grammar *g, const Item *item) {
    set ret = NULL;
    size_t prev = 0;
    size_t now;

    /* Expand for the first time */
    ret = Grammar_closure_once(g, item);
    if (!ret)
        return NULL;
    if (set_num(ret) == 1)
        return ret; /* If there were something to expand it would've already
                       happened */

    now = set_num(ret);

    /* Just keep expanding until there's no changes */
    /* I'm not proud at all to announce that this is in fact O(n^6) */
    while (now > prev) {
        size_t i, j;
        for (i = 0; i < now; ++i) {
            const Item *this = buffer_get(ret, i, Item);
            set aux = Grammar_closure_once(g, this); /* set<Item> */
            if (!aux)
                continue; /* nothing to do here */
            if (set_num(aux) == 1) {
                /* No new expansions */
                set_out(&buffer_get(aux, 0, Item)->look);
                set_out(&aux);
                continue;
            }
            /* add the new ones */
            for (j = 0; j < set_num(aux); ++j) {
                Item *brainrot = buffer_get(aux, j, Item);
                if (!hasitem(&ret, brainrot))
                    set_add(ret, brainrot);
                else
                    set_out(&brainrot->look);
            }
            set_out(&aux);
        }

        prev = now;
        now = set_num(ret);
    }

    return ret;
}
