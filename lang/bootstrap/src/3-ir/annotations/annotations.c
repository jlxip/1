#include "annotations.h"
#include "../lookup/lookup.h"

static ObjAnnotation walk_annotation(WalkCtx *ctx, AST *ast);
static buffer walk_generic(WalkCtx *ctx, AST *ast);
static buffer walk_primary_list(WalkCtx *ctx, AST *ast);

ObjAnnotations walk_annotations(WalkCtx *ctx, AST *ast) {
    ObjAnnotations ret = NULL;

    if (IS_NAME("annot_null"))
        return ret;
    assert(IS_NAME("annot_rec"));

    map_new_string(&ret, sizeof(ObjAnnotation), NULL, NULL, NULL, NULL);
    for (;;) {
        ObjAnnotation ann = walk_annotation(ctx, AST(SUB(0)));
        map_add(ret, TOKEN(ann.name)->data.str, &ann);

        ast = AST(SUB(1));
        if (IS_NAME("annot_null"))
            break;
        assert(IS_NAME("annot_rec"));
    }

    return ret;
}

static ObjAnnotation walk_annotation(WalkCtx *ctx, AST *ast) {
    ObjAnnotation ret;
    ret.name = SUB(1);
    ret.args = NULL;

    if (IS_NAME("annot")) {
        /* Nothing to do here */
    } else if (IS_NAME("annot_args")) {
        if (0 == strcmp(TOKEN(ret.name)->data.str, "generic")) {
            /* Args are undefined */
            ret.args = walk_generic(ctx, AST(SUB(3)));
        } else {
            /* In all other cases args are expected to be defined */
            ret.args = walk_primary_list(ctx, AST(SUB(3)));
        }
    } else
        UNREACHABLE;

    return ret;
}

static buffer walk_generic(WalkCtx *ctx, AST *ast) {
    buffer ret = NULL; /* buffer<char*> */

    assert(IS_NAME("primary_list_direct"));
    ast = AST(SUB(0)); /* PRIMARY */
    assert(IS_NAME("primary_id"));

    buffer_new(&ret, sizeof(char *));
    buffer_push(ret, TOKEN(SUB(0))->data.str);

    return ret;
}

static buffer walk_primary_list(WalkCtx *ctx, AST *ast) {
    buffer ret = NULL; /* buffer<Declaration*> */
    buffer_new(&ret, sizeof(Declaration *));

    for (;;) {
        Declaration *decl = lookup(ctx, AST(SUB(0)));
        buffer_push(ret, &decl);

        if (IS_NAME("primary_list_direct")) {
            break;
        } else if (IS_NAME("primary_list_rec")) {
            ast = AST(SUB(2));
        } else
            UNREACHABLE;
    }

    return ret;
}
