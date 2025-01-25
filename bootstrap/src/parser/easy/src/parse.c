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

void Grammar_parse(Grammar *g, const StreamElement *stream) {
    buffer stack = NULL;    /* buffer<state> */
    buffer valstack = NULL; /* buffer<void*> */

#ifdef DEBUG
    const StreamElement *dbgtokens = stream;
    printf("Tokens: ");
    while (dbgtokens->sym) {
        print_symbol(g, dbgtokens->sym);
        printf(" ");
        ++dbgtokens;
    }
    printf("\n");
#endif

    assert(g);
    assert(stream);

    if (!g->table)
        Grammar_compile(g);

    Grammar_clean(g);
    buffer_new(&stack, sizeof(state));
    buffer_new(&valstack, sizeof(void *));
    buffer_push(stack, &ZERO);
    while (stream->sym) {
        state st;
        const map *row;
        const Entry *entry;

        st = *buffer_back(stack, state);
        row = buffer_get(g->table, st, map);
        if (!map_has(*row, &stream->sym))
            unexpected_token(g, row, stream->sym);

        entry = map_get(*row, &stream->sym, Entry);
        switch (entry->type) {
        case ENTRY_ACCEPT:
            todo();
            break;
        case ENTRY_REDUCE: {
            size_t prodidx;
            const Production *prod;
            size_t x;
            buffer sub = NULL; /* buffer<void*> */
            void *val = NULL;

            /* Pop production while constructing sub */
            prodidx = entry->info;
            prod = buffer_get(g->g, prodidx, Production);
            x = buffer_num(prod->rhs);
            buffer_new(&sub, sizeof(void *));
            while (x--) {
                buffer_pop(stack);
                buffer_push(sub, buffer_back(valstack, void *));
                buffer_pop(valstack);
            }

            /* Perform GOTO */
            st = *buffer_back(stack, state);
            row = buffer_get(g->table, st, map);
            assert(map_has(*row, &prod->lhs));
            entry = map_get(*row, &prod->lhs, Entry);
            assert(entry->type == ENTRY_GOTO);
            buffer_push(stack, &entry->info);

            /* Output */
            if (*buffer_get(g->outputs, prodidx, void *)) {
                sdt_callback x = *buffer_get(g->outputs, prodidx, sdt_callback);
                val = x(buffer_get_raw(sub, void *), buffer_num(sub));
            } else {
#ifdef DEBUG
                printf("WARNING: no output defined for \"%s\"\n",
                    *buffer_get(g->prod2name, prodidx, char *));
#endif
            }
            buffer_push(valstack, &val);
            buffer_out(&sub);
            break;
        }
        case ENTRY_SHIFT:
            buffer_push(stack, &entry->info);
            buffer_push(valstack, &stream->data);
            ++stream;
            break;
        default:
            UNREACHABLE;
        }
    }

    buffer_out(&stack);
    buffer_out(&valstack);
}
