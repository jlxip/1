#include "internal.h"

void print_symbol(const Grammar *g, symbol sym) {
    if (!g->strtokens)
        throw("Cannot print symbol, no debugging information");

    if (!sym)
        printf("EPSILON");
    else if (IS_TERMINAL(g, sym))
        printf("%s", g->strtokens[sym - 1]);
    else
        printf("%s", g->strnts[sym - g->ntok - 1]);
}
