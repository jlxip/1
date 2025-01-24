#include "internal.h"

static const state ZERO = 0;

static void unexpected_token(const Grammar *g, const map *row, symbol sym) {
    size_t i;
    buffer b = NULL;

    printf("\nUnexpected token: ");
    print_symbol(g, sym);
    printf("\n");

    buffer_new(&b, sizeof(symbol));
    for (i = 1; i < g->ntok; ++i)
        if (map_has(*row, &i))
            buffer_push(b, &i);

    if (buffer_num(b) == 0) {
        printf("Expected end of file\n");
    } else if (buffer_num(b) == 1) {
        printf("Expected: ");
        print_symbol(g, *buffer_get(b, 0, symbol));
        printf("\n");
    } else {
        printf("Expected one of these:\n");
        for (i = 0; i < buffer_num(b); ++i) {
            printf("- ");
            print_symbol(g, *buffer_get(b, i, symbol));
            printf("\n");
        }
    }

    buffer_out(&b);
    exit(2);
}

void Grammar_parse(Grammar *g, const symbol *stream, void *data) {
    buffer stack = NULL;
    symbol sym;

#ifdef DEBUG
    const symbol *dbgtokens = stream;
    printf("Tokens: ");
    while (*dbgtokens) {
        print_symbol(g, *dbgtokens++);
        printf(" ");
    }
    printf("\n");
#endif

    (void)data;
    assert(g);
    assert(stream);

    if (!g->table)
        Grammar_compile(g);

    Grammar_clean(g);
    buffer_new(&stack, sizeof(state));
    buffer_push(stack, &ZERO);
    while ((sym = *stream)) {
        state st;
        const map *row;
        const Entry *entry;

        st = *buffer_back(stack, state);
        row = buffer_get(g->table, st, map);
        if (!map_has(*row, &sym))
            unexpected_token(g, row, sym);

        entry = map_get(*row, &sym, Entry);
        switch (entry->type) {
        case ENTRY_ACCEPT:
            todo();
            break;
        case ENTRY_REDUCE: {
            const Production *prod;
            size_t x;

            /* Pop production */
            prod = buffer_get(g->g, entry->info, Production);
            x = buffer_num(prod->rhs);
            while (x--)
                buffer_pop(stack);

            /* Perform GOTO */
            st = *buffer_back(stack, state);
            row = buffer_get(g->table, st, map);
            assert(map_has(*row, &prod->lhs));
            entry = map_get(*row, &prod->lhs, Entry);
            assert(entry->type == ENTRY_GOTO);
            buffer_push(stack, &entry->info);

            /* TODO: output here */
            break;
        }
        case ENTRY_SHIFT:
            buffer_push(stack, &entry->info);
            ++stream;
            break;
        default:
            UNREACHABLE;
        }
    }

    buffer_out(&stack);
}
