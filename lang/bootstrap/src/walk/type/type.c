#include "type.h"
#include <tokens.h>

Type walk_type(AST *ast, const char **names, Symbols *syms) {
    Type ret;

    (void)syms;

    if (IS_NAME("type_bool")) {
        ret.id = TYPE_BOOL;
        ret.data = NULL;
    } else if (IS_NAME("type_float")) {
        ret.id = TYPE_FLOAT;
        ret.data = NULL;
    } else if (IS_NAME("type_ptr")) {
        ret.id = TYPE_PTR;
        ret.data = NULL;
    } else if (IS_NAME("type_string")) {
        ret.id = TYPE_STRING;
        ret.data = NULL;
    } else if (IS_NAME("type_word")) {
        ret.id = TYPE_WORD;
        ret.data = NULL;
    } else if (IS_NAME("type_direct")) {
        /*Capture *id = (Capture *)(SUB_AST(0));*/
        todo();
    } else
        UNREACHABLE;

    return ret;
}
