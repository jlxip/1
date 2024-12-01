#include "Grammar.h"
#include <stdarg.h>
#include <stdlib.h>

void Grammar_new(Grammar *g, size_t ntok, size_t start) {
    g->g = NULL;
    buffer_new(&g->g, sizeof(Production));
    g->ntok = ntok;
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
    Production *x;

    /* Free production rules */
    for (buffer_iter(g->g, Production, x))
        buffer_out(&x->rhs);

    /* Free my G */
    buffer_out(&g->g);
}
