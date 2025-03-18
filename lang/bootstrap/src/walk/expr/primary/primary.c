#include "primary.h"
#include <tokens.h>

ObjPrimary walk_primary(AST *ast, const char **names, Symbols *syms) {
    ObjPrimary ret;

    (void)syms;

    if (IS_NAME("primary_direct")) {
        Capture *id = (Capture *)SUB_AST(0);
        assert(id->token == T_ID);
        ret.lineno = id->lineno;
        ret.name = (char *)(id->info);
    } else if (IS_NAME("primary_rec")) {
        todo();
    } else
        UNREACHABLE;

    return ret;
}
