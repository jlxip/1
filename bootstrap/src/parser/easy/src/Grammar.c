#include "Grammar.h"
#include <stdarg.h>
#include <stdlib.h>

/* --- Item --- */

Item Item_new(size_t prod, size_t dot, size_t nlook, ...) {
    va_list args;
    Item ret;
    ret.prod = prod;
    ret.dot = dot;
    ret.look = NULL;
    set_new_symbol(&ret.look);

    va_start(args, nlook);
    while (nlook--) {
        symbol x = va_arg(args, symbol);
        set_add(ret.look, &x);
    }
    va_end(args);
    return ret;
}

size_t hash_item(const void *ptr) {
    size_t ret;
    const Item *item = (const Item *)ptr;
    ret = hash_size_t(&item->prod);
    ret = combine_hashes(ret, hash_size_t(&item->dot));
    ret = combine_hashes(ret, hash_set(&item->look));
    return ret;
}

size_t hash_item_core(const void *ptr) {
    size_t ret;
    const Item *item = (const Item *)ptr;
    ret = hash_size_t(&item->prod);
    ret = combine_hashes(ret, hash_size_t(&item->dot));
    return ret;
}

size_t equal_item(const void *ptra, const void *ptrb) {
    const Item *a = (const Item *)ptra;
    const Item *b = (const Item *)ptrb;
    if (a->prod != b->prod)
        return 0;
    if (a->dot != b->dot)
        return 0;
    return equal_set(&a->look, &b->look);
}

size_t equal_item_core(const void *ptra, const void *ptrb) {
    const Item *a = (const Item *)ptra;
    const Item *b = (const Item *)ptrb;
    if (a->prod != b->prod)
        return 0;
    return a->dot == b->dot;
}

void *copy_item(const void *ptr) {
    const Item *item = (const Item *)ptr;
    Item *ret = malloc(sizeof(Item));
    ret->prod = item->prod;
    ret->dot = item->dot;
    ret->look = set_copy(item->look);
    return ret;
}

void destroy_item(void *ptr) {
    Item *item = (Item *)ptr;
    set_out(&item->look);
}

/* --- Entry --- */
size_t hash_entry(const void *ptr) {
    size_t ret;
    const Entry *entry = (const Entry *)ptr;
    ret = hash_size_t(&entry->type);
    ret = combine_hashes(ret, hash_size_t(&entry->info));
    return ret;
}

size_t equal_entry(const void *ptra, const void *ptrb) {
    const Entry *a = (const Entry *)ptra;
    const Entry *b = (const Entry *)ptrb;
    if (a->type != b->type)
        return 0;
    return a->info == b->info;
}

void *copy_entry(const void *ptr) {
    const Entry *entry = (const Entry *)ptr;
    Entry *ret = malloc(sizeof(Entry));
    ret->type = entry->type;
    ret->info = entry->info;
    return ret;
}

void destroy_entry(void *ptr) { (void)ptr; }

/* --- Grammar --- */

void Grammar_new(Grammar *g, size_t ntok, size_t nsym, symbol start) {
    g->g = NULL;
    buffer_new(&g->g, sizeof(Production));
    g->ntok = ntok;
    g->nsym = nsym;
    g->start = start;
    g->augmented = false;
    g->firsts = NULL;
    g->epsilons = NULL;
    g->collection = NULL;
    g->gotos = NULL;
    g->table = NULL;
}

void Grammar_add(Grammar *g, symbol lhs, buffer rhs) {
    Production prod;
    prod.lhs = lhs;
    prod.rhs = rhs;
    buffer_push(g->g, &prod);
}

void Grammar_shrink(Grammar *g) { buffer_shrink(g->g); }

void Grammar_augment(Grammar *g) {
    Production prod;

    if (g->augmented)
        throw("tried to augment an already-augmented grammar");

    /* Trick: use g->ntok as start symbol, keep 0 as epsilon */
    prod.lhs = g->ntok;
    prod.rhs = NULL;
    buffer_new(&prod.rhs, sizeof(symbol));
    buffer_push(prod.rhs, &g->start);

    /*
        Everything is clearer if we insert augmented start at the beginning,
        even though it's slower. Putting it at the end is much more confusing.
    */
    buffer_push_front(g->g, &prod);
    g->start = g->ntok;
    g->augmented = true;
}

void Grammar_out(Grammar *g) {
    size_t i;

    /* Free my G */
    for (i = 0; i < buffer_num(g->g); ++i) {
        Production *prod = buffer_get(g->g, i, Production);
        buffer_out(&prod->rhs);
    }
    buffer_out(&g->g);

    /* Free firsts: map<symbol, set<symbol>> */
    if (g->firsts)
        map_out(&g->firsts);

    /* Free epsilons: map<symbol, bool> */
    if (g->epsilons)
        map_out(&g->epsilons);

    /* Free collection: buffer<set<Item>> */
    if (g->collection) {
        for (i = 0; i < buffer_num(g->collection); ++i) {
            set *s = buffer_get(g->collection, i, set);
            set_out(s);
        }
        buffer_out(&g->collection);
    }

    /* Free gotos: buffer<map<symbol, state>> */
    if (g->gotos) {
        for (i = 0; i < buffer_num(g->gotos); ++i) {
            map *m = buffer_get(g->gotos, i, map);
            map_out(m);
        }
        buffer_out(&g->gotos);
    }

    /* Free table: buffer<map<symbol, Entry>> */
    if (g->table) {
        for (i = 0; i < buffer_num(g->table); ++i) {
            map *m = buffer_get(g->table, i, map);
            map_out(m);
        }
        buffer_out(&g->table);
    }
}
