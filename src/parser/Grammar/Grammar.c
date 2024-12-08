#include "Grammar.h"
#include <stdarg.h>
#include <stdlib.h>

void Grammar_new(Grammar *g, size_t ntok, size_t nsym, size_t start) {
    g->g = NULL;
    buffer_new(&g->g, sizeof(Production));
    g->ntok = ntok;
    g->nsym = nsym;
    g->start = start;
    g->augmented = 0;
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
    for (i = 0; i < buffer_num(g->firsts->b); ++i) {
        uint8_t *kv = buffer_get(g->firsts->b, i, void);
        set *v = (set *)(kv + g->firsts->ksize);
        set_out(v);
    }
    map_out(&g->firsts);
}
