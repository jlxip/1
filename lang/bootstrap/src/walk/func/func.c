#include "func.h"
#include "../block/block.h"
#include "../type/type.h"
#include <tokens.h>

ObjFunction walk_function(AST *ast, const char **names, Symbols *syms) {
    ObjFunction ret;
    Capture *id;

    /* TODO: annotations */

    /* Get the name */
    id = (Capture *)SUB_AST(2);
    assert(id->token == T_ID);
    ret.name = (char *)(id->info);
    ret.lineno = id->lineno;

    /* Parameters */
    if (IS_NAME("function_void") || IS_NAME("function_typed"))
        ret.params = walk_params(SUB_AST(4), names, syms);
    else
        ret.params = NULL;

    /* Return type */
    if (IS_NAME("function_noargs_typed")) {
        ret.ret = walk_type(SUB_AST(4), names, syms);
    } else if (IS_NAME("function_typed")) {
        ret.ret = walk_type(SUB_AST(7), names, syms);
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
    walk_block(ast, names, syms);

    return ret;
}
