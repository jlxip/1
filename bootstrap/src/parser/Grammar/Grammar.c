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
    set_new_size_t(&ret.look);

    va_start(args, nlook);
    while (nlook--) {
        size_t x = va_arg(args, size_t);
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

size_t equal_item(const void *ptra, const void *ptrb) {
    const Item *a = (const Item *)ptra;
    const Item *b = (const Item *)ptrb;
    if (a->prod != b->prod)
        return 0;
    if (a->dot != b->dot)
        return 0;
    return equal_set(&a->look, &b->look);
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

/* --- Grammar --- */

void Grammar_new(Grammar *g, size_t ntok, size_t nsym, size_t start) {
    g->g = NULL;
    buffer_new(&g->g, sizeof(Production));
    g->ntok = ntok;
    g->nsym = nsym;
    g->start = start;
    g->augmented = 0;
    g->firsts = NULL;
    g->epsilons = NULL;
    g->follows = NULL;
    g->collection = NULL;
}

void Grammar_add(Grammar *g, size_t lhs, size_t n, ...) {
    va_list args;
    Production prod;

    prod.lhs = lhs;
    prod.rhs = NULL;
    buffer_new(&prod.rhs, sizeof(size_t));

    va_start(args, n);
    while (n--) {
        size_t x = va_arg(args, size_t);
        buffer_push(prod.rhs, &x);
    }
    buffer_shrink(prod.rhs);

    va_end(args);
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
    buffer_new(&prod.rhs, sizeof(size_t));
    buffer_push(prod.rhs, &g->start);

    buffer_push(g->g, &prod);
    g->start = g->ntok;
    g->augmented = 1;
}

void Grammar_out(Grammar *g) {
    size_t i;

    /* Free my G */
    for (i = 0; i < buffer_num(g->g); ++i) {
        Production *prod = buffer_get(g->g, i, Production);
        buffer_out(&prod->rhs);
    }
    buffer_out(&g->g);

    /* Free firsts: map<size_t, set<size_t>> */
    if (g->firsts)
        map_out(&g->firsts);

    /* Free epsilons: map<size_t, size_t> */
    if (g->epsilons)
        map_out(&g->epsilons);

    /* Free follows: map<size_t, set<size_t>> */
    if (g->follows)
        map_out(&g->follows);

    /* Free collection: buffer<set<Item>> */
    if (g->collection) {
        for (i = 0; i < buffer_num(g->collection); ++i) {
            set *s = buffer_get(g->collection, i, set);
            set_out(s);
        }
        buffer_out(&g->collection);
    }
}
