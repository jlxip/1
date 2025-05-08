#include "semantics.h"

void push_scope(Ctx *ctx) {
    SymbolTable table = *buffer_get(ctx->tables, ctx->current, SymbolTable);
    Symbols syms = NULL;
    ShadowingTable shadow = NULL;

    map_new_string(&syms, sizeof(iIR), NULL, NULL, NULL, NULL);
    buffer_push(table, &syms);

    map_new_string(&shadow, sizeof(size_t), NULL, NULL, NULL, NULL);
    buffer_push(ctx->shadowing, &shadow);
}

void pop_scope(Ctx *ctx) {
    buffer_pop(*buffer_get(ctx->tables, ctx->current, SymbolTable));
    buffer_pop(ctx->shadowing);
}

static const size_t TWO = 2;
void push_to_scope(Ctx *ctx, const char *name, iIR iir) {
    SymbolTable table = *buffer_get(ctx->tables, ctx->current, SymbolTable);
    Symbols syms;
    ShadowingTable shadow = *buffer_back(ctx->shadowing, ShadowingTable);

    syms = *buffer_back(table, Symbols);
    map_remove_if_there(syms, name);
    map_add(syms, name, &iir);

    if (map_has(shadow, name))
        ++*map_get(shadow, name, size_t);
    else
        map_add(shadow, name, &TWO);
}

bool in_scope(Ctx *ctx, const char *name) {
    SymbolTable table;
    size_t scopes, i;

    table = *buffer_get(ctx->tables, ctx->current, SymbolTable);
    scopes = buffer_num(table);
    for (i = 0; i < scopes; ++i) {
        Symbols syms = *buffer_get(table, scopes - i - 1, Symbols);
        if (map_has(syms, name))
            return true;
    }

    return false;
}
