#include "annotations.h"
#include "../lookup/lookup.h"
#include <tokens.h>

static ObjAnnotation walk_annotation(
    AST *ast, const char **names, Symbols *syms);
static buffer walk_generic(AST *ast, const char **names);
static buffer walk_primary_list(AST *ast, const char **names, Symbols *syms);

ObjAnnotations walk_annotations(AST *ast, const char **names, Symbols *syms) {
    ObjAnnotations ret = NULL;

    if (IS_NAME("annot_null"))
        return ret;
    assert(IS_NAME("annot_rec"));

    map_new_string(&ret, sizeof(ObjAnnotation), NULL, NULL, NULL, NULL);
    for (;;) {
        ObjAnnotation ann = walk_annotation(SUB_AST(0), names, syms);
        map_add(ret, ann.name, &ann);

        ast = SUB_AST(1);
        if (IS_NAME("annot_null"))
            break;
        assert(IS_NAME("annot_rec"));
    }

    return ret;
}

static ObjAnnotation walk_annotation(
    AST *ast, const char **names, Symbols *syms) {
    Token *id;
    ObjAnnotation ret;

    (void)syms;

    id = (Token *)SUB_AST(1);
    assert(id->id == T_ID);
    ret.lineno = id->lineno;
    ret.name = id->data.str;
    ret.args = NULL;

    if (IS_NAME("annot")) {
        /* Nothing to do here */
    } else if (IS_NAME("annot_args")) {
        if (0 == strcmp(ret.name, "generic")) {
            /* Args are undefined */
            ret.args = walk_generic(SUB_AST(3), names);
        } else {
            /* In all other cases args are expected to be defined */
            ret.args = walk_primary_list(SUB_AST(3), names, syms);
        }
    } else
        UNREACHABLE;

    return ret;
}

static buffer walk_generic(AST *ast, const char **names) {
    buffer ret = NULL; /* buffer<char*> */
    Token *id;

    assert(IS_NAME("primary_list_direct"));
    ast = SUB_AST(0); /* PRIMARY */
    assert(IS_NAME("primary_id"));
    id = (Token *)SUB_AST(0);
    assert(id->id == T_ID);

    buffer_new(&ret, sizeof(char *));
    buffer_push(ret, &id->data.str);

    return ret;
}

static buffer walk_primary_list(AST *ast, const char **names, Symbols *syms) {
    buffer ret = NULL; /* buffer<Declaration*> */
    buffer_new(&ret, sizeof(Declaration *));

    for (;;) {
        Declaration *decl = lookup(SUB_AST(0), names, syms);
        buffer_push(ret, &decl);

        if (IS_NAME("primary_list_direct")) {
            break;
        } else if (IS_NAME("primary_list_rec")) {
            ast = SUB_AST(2);
        } else
            UNREACHABLE;
    }

    return ret;
}
