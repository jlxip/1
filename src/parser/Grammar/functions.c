#include "Grammar.h"

set get_first(const Grammar *g, size_t sym) {
    set visited = NULL;
    buffer remaining = NULL;
    set ret = NULL;

    if (!g->augmented)
        throw("tried to call get_first() on non-augmented grammar");

    set_new(&visited, sizeof(size_t));
    buffer_new(&remaining, sizeof(size_t));
    buffer_push(remaining, &sym);
    set_new(&ret, sizeof(size_t));

    while (!buffer_empty(remaining)) {
        /* Go through all production rules (SICK) */
        const Production *x;

        sym = *buffer_back(remaining, size_t);
        buffer_pop(remaining);
        if (set_has(visited, &sym))
            continue; /* already visited! */

        set_add(visited, &sym);
        if (IS_TERMINAL(g, sym)) {
            /* Got one! */
            set_add(ret, &sym);
            continue;
        }

        /* Non-terminal, check production rules */
        for (buffer_iter(g->g, Production, x)) {
            size_t next;

            /* Find sym in lhs */
            if (x->lhs != sym)
                continue; /* not interested */

            if (buffer_empty(x->rhs))
                next = EPSILON;
            else
                next = *buffer_front(x->rhs, size_t);
            buffer_push(remaining, &next);
        }
    }

    set_out(&visited);
    buffer_out(&remaining);
    return ret;
}
