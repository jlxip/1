/* Conflict resolution and printing */

#include "internal.h"

static void print_item(const Grammar *g, const Item *item) {
    const Production *prod;
    size_t i;

    prod = buffer_get(g->g, item->prod, Production);
    print_symbol(g, prod->lhs);
    printf(" ->");
    for (i = 0; i < buffer_num(prod->rhs); ++i) {
        if (item->dot == i)
            printf(" ·");
        printf(" ");
        print_symbol(g, *buffer_get(prod->rhs, i, symbol));
    }

    if (item->dot == END_OF_PRODUCTION)
        printf(" ·");
}

/* Prints the items of a state */
static void print_state(const Grammar *g, state st) {
    set_iterator it = set_it_begin(*buffer_get(g->collection, st, set));
    while (!set_it_finished(&it)) {
        printf("=> ");
        print_item(g, set_it_get(&it, Item));
        printf("\n");
        set_it_next(&it);
    }
}

/* Prints only the consumed items (dot at the end) of a state */
static void print_state_consumed(const Grammar *g, state st) {
    set_iterator it = set_it_begin(*buffer_get(g->collection, st, set));
    while (!set_it_finished(&it)) {
        const Item *item = set_it_get(&it, Item);
        if (item->dot != END_OF_PRODUCTION) {
            set_it_next(&it);
            continue;
        }

        printf("=> ");
        print_item(g, item);
        printf("\n");
        set_it_next(&it);
    }
}

static void could_not_solve_sr(
    const Grammar *g, state st, symbol sym, const char *err) {
    printf("\nSHIFT/REDUCE GRAMMAR CONFLICT\n");
    printf("%s\n", err);
    print_state_consumed(g, st);
    printf("Conflict with: ");
    print_symbol(g, sym);
    printf("\n");
    throw("grammar conflict");
}

/*
    Handles a shift/reduce conflict with precedence and associativity
    Returns:
    - 0 for shift
    - 1 for reduce
    Throws if unclear
*/
static size_t handle_shift_reduce(const Grammar *g, state st, symbol sym) {
    /*
        This is a token vs token situation
        State must have an item with the dot at the end of the production,
        and this production should have a token that indicates the conflict
        The other token is sym

        So, either prefer that token, or prefer sym
    */

    symbol tok;
    set_iterator it;
    const Precedence *prodprec, *symprec;
    size_t ret;

    tok = 0;
    it = set_it_begin(*buffer_get(g->collection, st, set));
    while (!set_it_finished(&it)) {
        const Item *item;
        const Production *prod;
        size_t i;

        item = set_it_get(&it, Item);
        if (item->dot != END_OF_PRODUCTION) {
            set_it_next(&it);
            continue;
        }

        prod = buffer_get(g->g, item->prod, Production);
        if (prod->hint) {
            tok = prod->hint;
            goto foundtok;
        }

        for (i = buffer_num(prod->rhs); i; --i) {
            symbol x = *buffer_get(prod->rhs, i - 1, symbol);
            if (IS_TERMINAL(g, x)) {
                tok = x;
                goto foundtok;
            }
        }
        set_it_next(&it);
    }

    if (!tok)
        could_not_solve_sr(g, st, sym, "No token in production (add hint)");

foundtok:
#ifdef DEBUG
    printf("Debug: shift/reduce: ");
    print_symbol(g, tok);
    printf(" vs ");
    print_symbol(g, sym);
    printf("\n");
#endif

    /* Look at prec now */
    prodprec = map_get(g->prec, &tok, Precedence);
    symprec = map_get(g->prec, &sym, Precedence);
    if (prodprec == NULL || symprec == NULL) {
        could_not_solve_sr(g, st, sym, "undefined associativity");
        ret = 69; /* cannot happen */
    } else if (prodprec->prio < symprec->prio) {
        /* Production's token has more priority, so reduce (1) */
        ret = 1;
    } else if (prodprec->prio > symprec->prio) {
        /* Symbol has more priority, so shift (0) */
        ret = 0;
    } else {
        /* Same priority, check associativty */
        switch (prodprec->assoc) {
        case UNDEFINED_ASSOC:
            could_not_solve_sr(g, st, sym, "undefined associativity");
            ret = 69; /* cannot happen */
            break;
        case NONASSOC:
            /* Non associative, doesn't matter the choice */
            ret = 0;
#ifdef DEBUG
            printf("Winner: none (non associative)\n");
#endif
            return ret;
        case LEFT_ASSOC:
            /* Left associative, so reduce (1) */
            ret = 1;
            break;
        case RIGHT_ASSOC:
            /* Right associative, so shift (0) */
            ret = 0;
            break;
        default:
            UNREACHABLE;
        }
    }

#ifdef DEBUG
    printf("Winner: ");
    if (ret == 0)
        printf("shift\n");
    else
        printf("reduce\n");
#endif

    return ret;
}

/*
    Handles a reduce/reduce conflict with %prefer
    Returns:
    - 0 for keep old
    - 1 for change
    Throws if unclear
*/
size_t handle_reduce_reduce(const Grammar *g, state st, symbol sym,
    const Entry *old, const Entry *new) {
    bool prefer_old, prefer_new;

    prefer_old = buffer_get(g->g, old->info, Production)->prefer;
    prefer_new = buffer_get(g->g, new->info, Production)->prefer;
    if (prefer_old > prefer_new)
        return 0;
    else if (prefer_old < prefer_new)
        return 1;

    printf("\nREDUCE/REDUCE GRAMMAR CONFLICT\n");
    if (!prefer_old && !prefer_new) {
        printf("This generally cannot be helped, fix your grammar\n");
        printf("If you know what you're doing, consider using %%prefer\n");
        printf("Using %%prefer has consequences, study in depth\n");
    } else if (prefer_old && prefer_new) {
        printf("Both possible reductions have %%prefer\n");
    }
    print_state(g, st);
    printf("Conflict with: ");
    print_symbol(g, sym);
    printf("\n");
    printf("Reduce to %lu or %lu?\n", old->info, new->info);
    throw("grammar conflict");
    return 69;
}

size_t handle_conflict(const Grammar *g, state st, symbol sym, const Entry *old,
    const Entry *new) {
    /*
        Found "old" in the table, but came up with a reduce. What to do?
        - If old is shift, this is a shift/reduce conflict
        - If old is reduce, this is a reduce/reduce conflict
    */

    if (old->type == ENTRY_SHIFT || old->type == ENTRY_GOTO)
        return handle_shift_reduce(g, st, sym);
    else
        return handle_reduce_reduce(g, st, sym, old, new);
}
