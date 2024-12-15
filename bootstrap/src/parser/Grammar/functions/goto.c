#include "../Grammar.h"

static Item advance(const Production *prod, const Item *parent) {
    /* Just advance the dot */
    Item ret;
    ret.prod = parent->prod;
    ret.dot = parent->dot + 1;
    if (ret.dot == buffer_num(prod->rhs))
        ret.dot = END_OF_PRODUCTION;
    ret.look = set_copy(parent->look);
    return ret;
}

set Grammar_goto(const Grammar *g, const set items, size_t sym) {
    set ret = NULL;   /* set<Item> */
    set gotos = NULL; /* set<Item> */
    set_iterator it;

    /* advance() everything that can be advanced with sym */
    set_new_Item(&gotos);
    it = set_it_begin(items);
    while (!set_it_finished(&it)) {
        const Item *parent = set_it_get(&it, Item);
        Item child;
        const Production *prod;
        size_t x;

        /* Is there something at the right of the dot? */
        if (parent->dot == END_OF_PRODUCTION) {
            /* nope */
            set_it_next(&it);
            continue;
        }

        prod = buffer_get(g->g, parent->prod, Production);
        x = *buffer_get(prod->rhs, parent->dot, size_t);
        if (x != sym) {
            /* not interested */
            set_it_next(&it);
            continue;
        }

        child = advance(prod, parent);
        set_add(gotos, &child);
        destroy_item(&child);
        set_it_next(&it);
    }

    /* Perform closure of all of them */
    set_new_Item(&ret);
    it = set_it_begin(gotos);
    while (!set_it_finished(&it)) {
        const Item *item = set_it_get(&it, Item);
        set aux = NULL;
        aux = Grammar_closure(g, item);
        if (aux) {
            set_join(ret, aux);
            set_out(&aux);
        }
        set_it_next(&it);
    }

    set_out(&gotos);
    return ret;
}
