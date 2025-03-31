#include "../type/type.h"
#include "func.h"
#include <tokens.h>

static Declaration *walk_param(AST *ast, const char **names, Symbols *syms);

buffer walk_params(AST *ast, const char **names, Symbols *syms) {
    buffer ret = NULL; /* buffer<Declaration*> */
    assert(IS_NAME("params_rec") || IS_NAME("params_one"));
    buffer_new(&ret, sizeof(Declaration));

    for (;;) {
        Declaration *param = walk_param(SUB_AST(0), names, syms);
        buffer_push(ret, &param);

        if (IS_NAME("params_one"))
            break;
        assert(IS_NAME("params_rec"));
        ast = SUB_AST(2);
    }

    return ret;
}

Declaration *walk_param(AST *ast, const char **names, Symbols *syms) {
    Declaration *ret;
    Capture *id;
    ret = malloc(sizeof(Declaration));

    if (IS_NAME("param_copy")) {
        ret->mut = ret->ref = false;
        ast = SUB_AST(0);
    } else if (IS_NAME("param_ref")) {
        ret->mut = false;
        ret->ref = true;
        ast = SUB_AST(1);
    } else if (IS_NAME("param_mut")) {
        ret->mut = true;
        ret->ref = false;
        ast = SUB_AST(1);
    } else if (IS_NAME("param_mutref")) {
        ret->mut = ret->ref = true;
        ast = SUB_AST(2);
    } else
        UNREACHABLE;

    assert(IS_NAME("typed"));
    id = (Capture *)SUB_AST(0);
    assert(id->token == T_ID);
    ret->lineno = id->lineno;
    ret->name = (const char *)(id->info);
    ret->type = walk_type(SUB_AST(2), names, syms);

    return ret;
}
