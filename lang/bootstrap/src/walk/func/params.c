#include "../type/type.h"
#include "func.h"

static Declaration *walk_param(WalkCtx *ctx, AST *ast);

buffer walk_params(WalkCtx *ctx, AST *ast) {
    buffer ret = NULL; /* buffer<Declaration*> */
    assert(IS_NAME("params_rec") || IS_NAME("params_one"));
    buffer_new(&ret, sizeof(Declaration));

    for (;;) {
        Declaration *param = walk_param(ctx, SUB_AST(0));
        buffer_push(ret, &param);

        if (IS_NAME("params_one"))
            break;
        assert(IS_NAME("params_rec"));
        ast = SUB_AST(2);
    }

    return ret;
}

static Declaration *walk_param(WalkCtx *ctx, AST *ast) {
    Declaration *ret;
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
    ret->name = (TokenIdx)SUB_AST(0);
    ret->type = walk_type(ctx, SUB_AST(2));

    return ret;
}
