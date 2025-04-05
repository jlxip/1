#include "lookup.h"
#include "../struct/struct.h"
#include "../type/type.h"

static buffer walk_types(WalkCtx *ctx, AST *ast);

Declaration *lookup(WalkCtx *ctx, AST *ast) {
    Declaration *ret;

    if (IS_NAME("primary_dot")) {
        todo();
    } else if (IS_NAME("primary_typed")) {
        ObjStruct *sd;
        ObjSpecificStruct *specific;

        ret = lookup(ctx, SUB_AST(0));
        if (ret->type.id != TYPE_STRUCT_DEF)
            throw("tried to specify a non-struct");
        sd = (ObjStruct *)(ret->type.data);
        if (sd->generic == NULL)
            throw("tried to specify a non-generic struct");

        specific = malloc(sizeof(ObjSpecificStruct));
        specific->struct_def = ret;
        specific->specific = walk_types(ctx, SUB_AST(2));

        if (buffer_num(sd->generic) != buffer_num(specific->specific))
            throw("incorrect number of types in specification");

        ret = malloc(sizeof(Declaration));
        ret->name = specific->struct_def->name;
        ret->type.id = TYPE_STRUCT_SPECIFIC;
        ret->type.data = specific;
        ret->mut = false;
    } else if (IS_NAME("primary_id")) {
        const char *name;
        bool found;
        size_t nsyms;
        size_t i;

        name = TOKEN((TokenIdx)SUB_AST(0))->data.str;

        found = false;
        nsyms = buffer_num(ctx->syms);
        for (i = 0; i < nsyms; ++i) {
            SymbolTable st = *buffer_get(ctx->syms, nsyms - i - 1, SymbolTable);
            if (map_has(st, name)) {
                ret = map_get(st, name, Declaration);
                found = true;
                break;
            }
        }

        if (!found)
            throwe("undeclared: %s", name);
    } else
        UNREACHABLE;

    return ret;
}

static buffer walk_types(WalkCtx *ctx, AST *ast) {
    buffer ret = NULL; /* buffer<Type> */
    buffer_new(&ret, sizeof(Type));

    assert(IS_NAME("types_direct") || IS_NAME("types_rec"));

    for (;;) {
        Type t;
        t = walk_type(ctx, SUB_AST(0));
        buffer_push(ret, &t);

        if (IS_NAME("types_direct"))
            break;
        assert(IS_NAME("types_rec"));
        ast = SUB_AST(2);
    }

    return ret;
}
