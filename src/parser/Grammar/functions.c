#include "Grammar.h"

/*
    SICK = Slow, I certainly know (but don't care)
    Most SICKs would be solved by a fast implementation of set, which is
    certainly not worth it for the bootstrap compiler. Keep in mind that this
    compiler is used exactly ONCE. The 1 implementation will be much better.
    Besides, writing a RB tree or an AVL in C89 is pure pain.
*/

buffer get_first(const Grammar *g, size_t sym) {
    buffer visited = NULL;
    buffer remaining = NULL;
    buffer ret = NULL;

    if (!g->augmented)
        throw("tried to call get_first() on non-augmented grammar");

    buffer_new(&visited, sizeof(size_t));
    buffer_new(&remaining, sizeof(size_t));
    buffer_push(remaining, &sym);
    buffer_new(&ret, sizeof(size_t));

    while (!buffer_empty(remaining)) {
        /* Go through all production rules (SICK) */
        const Production *x;

        sym = *buffer_back(remaining, size_t);
        buffer_pop(remaining);
        if (buffer_has(visited, &sym))
            continue; /* already visited! */

        buffer_push(visited, &sym);
        if (IS_TERMINAL(g, sym)) {
            /* Got one! */
            buffer_push(ret, &sym);
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

    buffer_out(&visited);
    buffer_out(&remaining);
    return ret;
}
