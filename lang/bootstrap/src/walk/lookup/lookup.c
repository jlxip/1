#include "lookup.h"
#include "../struct/struct.h"
#include "../type/type.h"
#include <tokens.h>

static buffer walk_types(AST *ast, const char **names, Symbols *syms);

Declaration *lookup(AST *ast, const char **names, Symbols *syms) {
    Declaration *ret;

    if (IS_NAME("primary_dot")) {
        todo();
    } else if (IS_NAME("primary_typed")) {
        ObjStruct *sd;
        ObjSpecificStruct *specific;

        ret = lookup(SUB_AST(0), names, syms);
        if (ret->type.id != TYPE_STRUCT_DEF)
            throw("tried to specify a non-struct");
        sd = (ObjStruct *)(ret->type.data);
        if (sd->generic == NULL)
            throw("tried to specify a non-generic struct");

        specific = malloc(sizeof(ObjSpecificStruct));
        specific->struct_def = ret;
        specific->specific = walk_types(SUB_AST(2), names, syms);

        if (buffer_num(sd->generic) != buffer_num(specific->specific))
            throw("incorrect number of types in specification");

        ret = malloc(sizeof(Declaration));
        ret->lineno = specific->struct_def->lineno;
        ret->name = specific->struct_def->name;
        ret->type.id = TYPE_STRUCT_SPECIFIC;
        ret->type.data = specific;
        ret->mut = false;
    } else if (IS_NAME("primary_id")) {
        Capture *id;
        const char *name;
        bool found;
        size_t nsyms;
        size_t i;

        id = (Capture *)SUB_AST(0);
        assert(id->token == T_ID);
        name = (const char *)(id->info);

        found = false;
        nsyms = buffer_num(*syms);
        for (i = 0; i < nsyms; ++i) {
            SymbolTable st = *buffer_get(*syms, nsyms - i - 1, SymbolTable);
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

static buffer walk_types(AST *ast, const char **names, Symbols *syms) {
    buffer ret = NULL; /* buffer<Type> */
    buffer_new(&ret, sizeof(Type));

    assert(IS_NAME("types_direct") || IS_NAME("types_rec"));

    for (;;) {
        Type t;
        t = walk_type(SUB_AST(0), names, syms);
        buffer_push(ret, &t);

        if (IS_NAME("types_direct"))
            break;
        assert(IS_NAME("types_rec"));
        ast = SUB_AST(2);
    }

    return ret;
}
