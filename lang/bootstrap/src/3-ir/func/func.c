#include "func.h"
#include "../block/block.h"
#include "../type/type.h"

ObjFunction walk_function(WalkCtx *ctx, AST *ast) {
    ObjFunction ret;

    /* TODO: annotations */

    /* Get the name */
    ret.name = SUB(2);

    /* Parameters */
    if (IS_NAME("function_void") || IS_NAME("function_typed"))
        ret.params = walk_params(ctx, AST(SUB(4)));
    else
        ret.params = NULL;

    /* Return type */
    if (IS_NAME("function_noargs_typed")) {
        ret.ret = walk_type(ctx, AST(SUB(4)));
    } else if (IS_NAME("function_typed")) {
        ret.ret = walk_type(ctx, AST(SUB(7)));
    } else {
        ret.ret.id = TYPE_NOTHING;
        ret.ret.data = NULL;
    }

    /* Walk the block */
    if (IS_NAME("function_noargs_void")) {
        ast = AST(SUB(3));
    } else if (IS_NAME("function_noargs_typed")) {
        ast = AST(SUB(5));
    } else if (IS_NAME("function_void")) {
        ast = AST(SUB(6));
    } else if (IS_NAME("function_typed")) {
        ast = AST(SUB(8));
    } else
        UNREACHABLE;

    assert(IS_NAME("block"));
    walk_block(ctx, ast);

    return ret;
}
