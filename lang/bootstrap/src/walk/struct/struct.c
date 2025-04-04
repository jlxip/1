#include "struct.h"
#include "../annotations/annotations.h"
#include "../type/type.h"

static map walk_struct_def(WalkCtx *ctx, AST *ast);

ObjStruct walk_struct(WalkCtx *ctx, AST *ast) {
    ObjStruct ret;
    ObjAnnotations anns;
    map fields;

    assert(IS_NAME("struct"));

    anns = walk_annotations(ctx, SUB_AST(0));
    if (anns) {
        if (map_has(anns, "generic")) {
            ObjAnnotation *generic = map_get(anns, "generic", ObjAnnotation);
            ret.generic = generic->args;
        } else {
            ret.generic = NULL;
        }
    } else {
        ret.generic = NULL;
    }

    ast = SUB_AST(4);
    fields = walk_struct_def(ctx, ast);

    ret.name = (TokenIdx)SUB_AST(2);
    ret.fields = fields;
    return ret;
}

static map walk_struct_def(WalkCtx *ctx, AST *ast) {
    map ret = NULL; /* map<const char*, Type> */
    if (IS_NAME("struct_def_null"))
        throw("empty struct");
    assert(IS_NAME("struct_def"));
    map_new_string(&ret, sizeof(Type), NULL, NULL, NULL, NULL);

    for (;;) {
        AST *next;
        const char *name;
        Type type;

        if (IS_NAME("struct_def_null"))
            break;
        assert(IS_NAME("struct_def"));

        next = SUB_AST(2);
        ast = SUB_AST(0);

        name = TOKEN((TokenIdx)SUB_AST(0))->data.str;
        type = walk_type(ctx, SUB_AST(2));

        if (map_has(ret, name))
            throwe("multiple fields named %s", name);
        map_add(ret, name, &type);
        ast = next;
    }

    return ret;
}
