#include "struct.h"
#include "../annotations/annotations.h"
#include <tokens.h>

void walk_struct_def(AST *ast, const char **names, Symbols *syms);

void walk_struct(AST *ast, const char **names, Symbols *syms) {
    ObjAnnotations anns;
    Capture *id;
    const char *name;

    anns = walk_annotations(SUB_AST(0), names, syms);
    (void)anns;

    id = (Capture *)SUB_AST(2);
    name = (const char *)(id->info);

    printf("Defining struct: %s\n", name);

    ast = SUB_AST(4);
    walk_struct_def(ast, names, syms);
}

void walk_struct_def(AST *ast, const char **names, Symbols *syms) {
    AST *aux;

    if (IS_NAME("struct_def_null"))
        return;
    assert(IS_NAME("struct_def"));

    aux = SUB_AST(0);
    do {
        AST *ast = aux;
        Capture *id;

        id = (Capture *)SUB_AST(0);
        assert(id->token == T_ID);
        printf("Element: %s\n", (const char *)(id->info));
    } while (0);

    ast = SUB_AST(2);
    walk_struct_def(ast, names, syms);
}
