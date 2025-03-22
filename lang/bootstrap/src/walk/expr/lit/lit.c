#include "lit.h"
#include <tokens.h>

ObjLiteral walk_lit(AST *ast, const char **names, Symbols *syms) {
    ObjLiteral ret;
    Capture *capture = (Capture *)SUB_AST(0); /* applies to all */
    (void)syms;

    if (IS_NAME("lit_bool")) {
        todo();
    } else if (IS_NAME("lit_word")) {
        assert(capture->token == T_WORD);
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
