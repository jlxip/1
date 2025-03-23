#include "impl.h"
#include "../annotations/annotations.h"
#include "../func/func.h"

static map walk_impl_def(AST *ast, const char **names, Symbols *syms);

void walk_impl(AST *ast, const char **names, Symbols *syms) {
    ObjAnnotations anns;

    anns = walk_annotations(SUB_AST(0), names, syms);
    (void)anns;

    walk_impl_def(SUB_AST(4), names, syms);
}

static map walk_impl_def(AST *ast, const char **names, Symbols *syms) {
    map ret = NULL; /* map<const char*, ObjFunction> */

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
}
