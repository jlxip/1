#include "lit.h"

ObjLiteral walk_lit(WalkCtx *ctx, AST *ast) {
    ObjLiteral ret;

    if (IS_NAME("lit_bool")) {
        todo();
    } else if (IS_NAME("lit_word")) {
        ret.mst = SUB(0);
        ret.type.id = TYPE_WORD;
        ret.type.data = NULL;
    } else if (IS_NAME("lit_float")) {
        todo();
    } else if (IS_NAME("lit_string")) {
        todo();
    } else
        UNREACHABLE;

    return ret;
}
