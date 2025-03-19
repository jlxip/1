#include "annotations.h"
#include "../expr/primary/primary.h"
#include <tokens.h>

static ObjAnnotation walk_annotation(
    AST *ast, const char **names, Symbols *syms);
static buffer walk_primary_list(AST *ast, const char **names, Symbols *syms);

ObjAnnotations walk_annotations(AST *ast, const char **names, Symbols *syms) {
    ObjAnnotations ret = NULL;

    if (IS_NAME("annot_null"))
        return ret;

    buffer_new(&ret, sizeof(ObjAnnotation));
    for (;;) {
        ObjAnnotation ann = walk_annotation(SUB_AST(0), names, syms);
        buffer_push(ret, &ann);

        ast = SUB_AST(1);
        if (IS_NAME("annot_null"))
            break;
    }

    return ret;
}

static ObjAnnotation walk_annotation(
    AST *ast, const char **names, Symbols *syms) {
    Capture *id;
    ObjAnnotation ret;

    (void)syms;

    id = (Capture *)SUB_AST(1);
    assert(id->token == T_ID);
    ret.lineno = id->lineno;
    ret.name = (const char *)(id->info);
    ret.args = NULL;

    if (IS_NAME("annot")) {
        /* Nothing to do here */
    } else if (IS_NAME("annot_args")) {
        ret.args = walk_primary_list(SUB_AST(3), names, syms);
    } else
        UNREACHABLE;

    return ret;
}

static buffer walk_primary_list(AST *ast, const char **names, Symbols *syms) {
    buffer ret = NULL;
    buffer_new(&ret, sizeof(ObjPrimary));

    for (;;) {
        ObjPrimary primary = walk_primary(SUB_AST(0), names, syms);
        buffer_push(ret, &primary);

        if (IS_NAME("primary_list_direct")) {
            break;
        } else if (IS_NAME("primary_list_rec")) {
            ast = SUB_AST(2);
        } else
            UNREACHABLE;
    }

    return ret;
}
