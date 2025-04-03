#include "lit.h"

ObjLiteral walk_lit(WalkCtx *ctx, AST *ast) {
    ObjLiteral ret;
    Token *capture = (Token *)SUB_AST(0); /* applies to all */

    if (IS_NAME("lit_bool")) {
        todo();
    } else if (IS_NAME("lit_word")) {
        assert(capture->id == T_WORD);
        ret.lineno = capture->lineno;
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
