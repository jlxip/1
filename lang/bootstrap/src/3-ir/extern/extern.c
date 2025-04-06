#include "extern.h"
#include "../func/func.h"
#include "../type/type.h"

ObjExtern walk_extern(WalkCtx *ctx, AST *ast) {
    ObjExtern ret;

    /* Get the name */
    ret.name = SUB(1);

    /* Parameters */
    if (IS_NAME("extern_void") || IS_NAME("extern_typed"))
        ret.params = walk_params(ctx, AST(SUB(3)));
    else
        ret.params = NULL;

    /* Return type */
    if (IS_NAME("extern_noargs_typed")) {
        ret.ret = walk_type(ctx, AST(SUB(3)));
    } else if (IS_NAME("extern_typed")) {
        ret.ret = walk_type(ctx, AST(SUB(6)));
    } else {
        ret.ret.id = TYPE_NOTHING;
        ret.ret.data = NULL;
    }

    return ret;
}
