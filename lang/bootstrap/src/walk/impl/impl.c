#include "impl.h"
#include "../annotations/annotations.h"
#include "../func/func.h"
#include "../lookup/lookup.h"

void walk_impl(WalkCtx *ctx, AST *ast) {
    /* ANNOTATIONS impl PRIMARY obraces IMPL_DEF cbraces  */
    Declaration *decl;

    /* TODO: annotations */

    decl = lookup(ctx, SUB_AST(2));
    if (decl->type.id == TYPE_STRUCT_DEF) {
        todo();
    } else if (decl->type.id == TYPE_STRUCT_SPECIFIC) {
        todo();
    } else {
        throw("can only implement a struct");
    }

    NEW_SCOPE;
    PUSH_TO_SCOPE("Self", decl);
    POP_SCOPE;

    /*walk_impl_def(SUB_AST(4), names, syms);*/
    todo();
}

/*static map walk_impl_def(AST *ast, const char **names, Symbols *syms);

void walk_impl(AST *ast, const char **names, Symbols *syms) {
    Declaration *decl;
    Declaration self;
    ObjAnnotations anns;

    decl = lookup(SUB_AST(2), names, syms);
    assert(decl->type.id == TYPE_STRUCT_DEF);

    anns = walk_annotations(SUB_AST(0), names, syms);
    (void)anns;

    walk_impl_def(SUB_AST(4), names, syms);
}

static map walk_impl_def(AST *ast, const char **names, Symbols *syms) {
    map ret = NULL;

    if (IS_NAME("impl_def_null"))
        throw("empty impl");

    map_new_string(&ret, sizeof(ObjFunction), NULL, NULL, NULL, NULL);

    for (;;) {
        ObjFunction fn;

        if (IS_NAME("impl_def_null"))
            break;

        fn = walk_function(SUB_AST(0), names, syms);
        map_add(ret, fn.name, &fn);

        ast = SUB_AST(1);
    }

    return ret;
}*/
