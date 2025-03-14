#include "func.h"
#include "../block/block.h"
#include <tokens.h>

ObjFunction walk_function(AST *ast, const char **names, Symbols *syms) {
    ObjFunction ret;
    char *name = NULL;
    size_t lineno = 0;

    /* TODO: annotations */

    /* Get the name */
    do {
        Capture *id = (Capture *)SUB_AST(2);
        assert(id->token == T_ID);
        name = (char *)(id->info);
        lineno = id->lineno;
    } while (0);

    /* TODO: arguments */

    /* Walk the block */
    ast = SUB_AST(6);
    assert(IS_NAME("block"));
    walk_block(ast, names, syms);

    ret.lineno = lineno;
    ret.name = name;

    return ret;
}
