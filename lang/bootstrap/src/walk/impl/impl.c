#include "impl.h"
#include "../annotations/annotations.h"
#include "../func/func.h"
#include "../lookup/lookup.h"

void walk_impl(AST *ast, const char **names, Symbols *syms) {
    (void)ast;
    (void)names;
    (void)syms;
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
