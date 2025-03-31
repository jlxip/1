#include "extern.h"
#include "../func/func.h"
#include "../type/type.h"
#include <tokens.h>

ObjExtern walk_extern(AST *ast, const char **names, Symbols *syms) {
    ObjExtern ret;
    Capture *id;

    /* Get the name */
    id = (Capture *)(SUB_AST(1));
    assert(id->token == T_ID);
    ret.lineno = id->lineno;
    ret.name = (const char *)(id->info);

    /* Parameters */
    if (IS_NAME("extern_void") || IS_NAME("extern_typed"))
        ret.params = walk_params(SUB_AST(3), names, syms);
    else
        ret.params = NULL;

    /* Return type */
    if (IS_NAME("extern_noargs_typed")) {
        ret.ret = walk_type(SUB_AST(3), names, syms);
    } else if (IS_NAME("extern_typed")) {
        ret.ret = walk_type(SUB_AST(6), names, syms);
    } else {
        ret.ret.id = TYPE_NOTHING;
        ret.ret.data = NULL;
    }

    return ret;
}
