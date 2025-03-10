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

AST Grammar_parse(Grammar *g, const StreamElement *stream) {
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
    for (;;) {
        symbol sym;
        state st;
        const map *row;
        const Entry *entry;

        if (!stream->sym)
            sym = g->nsym;
        else
            sym = stream->sym;

        st = *buffer_back(stack, state);
        row = buffer_get(g->table, st, map);
        if (!map_has(*row, &sym))
            unexpected_token(g, row, sym);

        entry = map_get(*row, &sym, Entry);
        switch (entry->type) {
        case ENTRY_ACCEPT: {
            AST ret = **buffer_back(valstack, AST *);
            buffer_out(&valstack);
            buffer_out(&stack);
            return ret;
        }
        case ENTRY_REDUCE: {
            size_t prodidx;
            const Production *prod;
            size_t x;
            buffer sub = NULL; /* buffer<void*> */
            AST *ast;

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
            buffer_reverse(sub);

            /* Perform GOTO */
            st = *buffer_back(stack, state);
            row = buffer_get(g->table, st, map);
            assert(map_has(*row, &prod->lhs));
            entry = map_get(*row, &prod->lhs, Entry);
            assert(entry->type == ENTRY_GOTO);
            buffer_push(stack, &entry->info);

            /* Output (create AST element) */
            ast = malloc(sizeof(AST));
            ast->prod = prodidx - 1;
            ast->sub = sub;
            buffer_push(valstack, &ast);
            break;
        }
        case ENTRY_SHIFT:
            assert(stream->sym != 0);
            buffer_push(stack, &entry->info);
            buffer_push(valstack, &stream->data);
            ++stream;
            break;
        default:
            UNREACHABLE;
        }
    }

    UNREACHABLE;
}
