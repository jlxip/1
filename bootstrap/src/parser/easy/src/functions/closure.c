#include "../internal.h"

static const symbol EEPSILON = EPSILON;

/* LR(1) item closure: [A -> alpha · B beta, a] */
/* This expands only once */
static set Grammar_closure_once(Grammar *g, const Item *item) {
    set ret = NULL; /* set<Item> */
    const Production *prod;
    symbol sym;
    size_t i;
    buffer beta = NULL; /* buffer<symbol> */

    set_new_Item(&ret);
    set_add(ret, item);

    /* If dot is at the end of item, no additional closure */
    if (item->dot == END_OF_PRODUCTION)
        return ret;

    /* Get symbol next to the dot */
    prod = buffer_get(g->g, item->prod, Production);
    sym = *buffer_get(prod->rhs, item->dot, symbol);

    /* If it's a terminal, we're done, return item */
    if (IS_TERMINAL(g, sym))
        return ret;

    /* beta = prod[dot+1:] */
    buffer_new(&beta, sizeof(symbol));
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

        /* Epsilon has no symbols so the dot is at the end */
        if (buffer_empty(p->rhs))
            new->dot = END_OF_PRODUCTION;

        /* Now compute second component, FIRST(beta a) */
        new->look = FIRST_MANY(g, beta);
        if (set_empty(new->look)) {
            /* No firsts; so, epsilon. Add a */
            set_join(new->look, item->look);
        } else if (set_has(new->look, &EEPSILON)) {
            /* Epsilon in new: remove it and add a */
            set_remove(new->look, &EEPSILON);
            set_join(new->look, item->look);
        }

        set_add_move(ret, new);
    }

    buffer_out(&beta);
    return ret;
}

static set join_items(set s) {
    set ret = NULL; /* set<Item (core)> */
    set_iterator it;
    set ret2 = NULL; /* set<Item> */

    set_new_Item_core(&ret);
    it = set_it_begin(s);
    while (!set_it_finished(&it)) {
        Item *item = set_it_get(&it, Item);
        if (set_has(ret, item)) {
            /* Already there, join lookahead symbols */
            set_join(set_get(ret, item, Item)->look, item->look);
        } else {
            /* Just add */
            set_add(ret, item);
        }
        set_it_next(&it);
    }

    set_out(&s);

    /* Return set<Item> instead of set<Item (core)> */
    set_new_Item(&ret2);
    set_migrate(ret2, ret);
    return ret2;
}

static void push_copy(buffer x, const Item *y) {
    Item *copy = copy_item(y);
    buffer_push(x, copy);
    free(copy); /* shallow free */
}

/* Acutal iterative LR(1) item closure */
set CLOSURE(Grammar *g, const Item *item) {
    set result = NULL; /* set<Item> */
    buffer new = NULL; /* buffer<Item> (stack-like) */

    if (!g->augmented)
        Grammar_augment(g);

    if (g->closure_cache == NULL) {
        map_new(&g->closure_cache, hash_item, equal_item, copy_item,
            destroy_item, sizeof(set), hash_set, equal_set, copy_set,
            destroy_set);
    } else if (map_has(g->closure_cache, item)) {
        return set_copy(*map_get(g->closure_cache, item, set));
    }

    set_new_Item(&result);
    buffer_new(&new, sizeof(Item));
    push_copy(new, item);

    /* Keep expanding every new item */
    while (!buffer_empty(new)) {
        Item this;
        set expansion; /* set<Item> */
        set_iterator it;

        /* Get an item from new */
        this = *buffer_back(new, Item);
        buffer_pop(new);
        if (set_has(result, &this)) {
            /* Already seen */
            destroy_item(&this);
            continue;
        }

        /* Expand it */
        set_add(result, &this);
        expansion = Grammar_closure_once(g, &this);
        destroy_item(&this);

        /* Check everything not seen */
        it = set_it_begin(expansion);
        while (!set_it_finished(&it)) {
            push_copy(new, set_it_get(&it, void));
            set_it_next(&it);
        }

        set_out(&expansion);
    }

    buffer_out(&new);
    result = join_items(result);
    map_add(g->closure_cache, item, &result);
    return result;
}
