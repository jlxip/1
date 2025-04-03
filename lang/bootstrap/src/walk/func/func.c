#include "func.h"
#include "../block/block.h"
#include "../type/type.h"

ObjFunction walk_function(WalkCtx *ctx, AST *ast) {
    ObjFunction ret;
    Token *id;

    /* TODO: annotations */

    /* Get the name */
    id = (Token *)SUB_AST(2);
    assert(id->id == T_ID);
    ret.name = id->data.str;
    ret.lineno = id->lineno;

    /* Parameters */
    if (IS_NAME("function_void") || IS_NAME("function_typed"))
        ret.params = walk_params(ctx, SUB_AST(4));
    else
        ret.params = NULL;

    /* Return type */
    if (IS_NAME("function_noargs_typed")) {
        ret.ret = walk_type(ctx, SUB_AST(4));
    } else if (IS_NAME("function_typed")) {
        ret.ret = walk_type(ctx, SUB_AST(7));
    } else {
        ret.ret.id = TYPE_NOTHING;
        ret.ret.data = NULL;
    }

    /* Walk the block */
    if (IS_NAME("function_noargs_void")) {
        ast = SUB_AST(3);
    } else if (IS_NAME("function_noargs_typed")) {
        ast = SUB_AST(5);
    } else if (IS_NAME("function_void")) {
        ast = SUB_AST(6);
    } else if (IS_NAME("function_typed")) {
        ast = SUB_AST(8);
    } else
        UNREACHABLE;

    assert(IS_NAME("block"));
    walk_block(ctx, ast);

    return ret;
}
