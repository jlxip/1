#include "internal.h"

static const size_t ZERO = 0;
static const IR IR_token = {NULL, NULL};

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

IRs Grammar_parse(Grammar *g, const StreamElement *stream) {
    IRs ret = NULL;
    buffer stack = NULL;    /* buffer<state> */
    buffer valstack = NULL; /* buffer<size_t (iToken/iIR)> */
    buffer irstack = NULL;  /* buffer<iIR> */

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
    buffer_new(&ret, sizeof(IR));
    buffer_new(&stack, sizeof(state));
    buffer_new(&valstack, sizeof(size_t));
    buffer_new(&irstack, sizeof(iIR));
    buffer_push(ret, &IR_token);
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
        case ENTRY_ACCEPT:
            buffer_shrink(ret);
            buffer_out(&valstack);
            buffer_out(&stack);
            buffer_out(&irstack);
            return ret;
        case ENTRY_REDUCE: {
            size_t prodidx;
            const Production *prod;
            size_t x;
            IR ir = {NULL, NULL};
            iIR iir;

            /* Pop production while constructing ir */
            prodidx = entry->info;
            prod = buffer_get(g->g, prodidx, Production);
            x = buffer_num(prod->rhs);
            buffer_new(&ir.types, sizeof(size_t));
            buffer_new(&ir.ids, sizeof(size_t));
            while (x--) {
                iIR child;
                size_t childval;
                child = *buffer_back(irstack, size_t);
                buffer_pop(irstack);
                childval = *buffer_back(valstack, size_t);
                buffer_pop(valstack);
                buffer_pop(stack);

                if (child == 0) {
                    /* This is a token */
                    buffer_push(ir.types, &ZERO);
                    buffer_push(ir.ids, &childval);
                } else {
                    /* This is a NT */
                    buffer_push(ir.types, &childval);
                    buffer_push(ir.ids, &child);
                }
            }

            buffer_reverse(ir.types);
            buffer_reverse(ir.ids);

            /* Perform GOTO */
            st = *buffer_back(stack, state);
            row = buffer_get(g->table, st, map);
            assert(map_has(*row, &prod->lhs));
            entry = map_get(*row, &prod->lhs, Entry);
            assert(entry->type == ENTRY_GOTO);
            buffer_push(stack, &entry->info);

#ifdef DEBUG
            printf("Reduce\n");
#endif

            buffer_push(valstack, &prodidx);
            iir = buffer_num(ret);
            buffer_push(ret, &ir);
            buffer_push(irstack, &iir);
            break;
        }
        case ENTRY_SHIFT:
            assert(stream->sym != 0);
#ifdef DEBUG
            printf("Shift: ");
            print_symbol(g, sym);
            printf("\n");
#endif
            buffer_push(stack, &entry->info);
            buffer_push(valstack, &stream->data);
            buffer_push(irstack, &ZERO);
            ++stream;
            break;
        default:
            UNREACHABLE;
        }
    }

    UNREACHABLE;
}
