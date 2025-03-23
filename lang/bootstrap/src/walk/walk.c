#include "walk.h"
#include "block/decl/decl.h"
#include "expr/expr.h"
#include "func/func.h"
#include "impl/impl.h"
#include "struct/struct.h"
#include <stdio.h>
#include <tokens.h>

void walk_uses(AST *ast, const char **names, Symbols *syms);
void walk_globals(AST *ast, const char **names, Symbols *syms);
void walk_global(AST *ast, const char **names, Symbols *syms);

void walk(ASTRoot root) {
    AST *ast = &root.ast;
    char **names = root.names;
    Symbols _syms = NULL;
    Symbols *syms = NULL;

    assert(IS_NAME("program"));

    /* Initialize syms and create global scope */
    buffer_new(&_syms, sizeof(map));
    syms = &_syms;
    NEW_SCOPE;

    walk_uses(SUB_AST(0), (const char **)names, syms);
    walk_globals(SUB_AST(1), (const char **)names, syms);
}

void walk_uses(AST *ast, const char **names, Symbols *syms) {
    (void)syms;
    if (!IS_NAME("uses_null"))
        todo();
}

void walk_globals(AST *ast, const char **names, Symbols *syms) {
    AST *global, *globals;

    if (IS_NAME("globals_null"))
        return;
    assert(IS_NAME("globals_rec"));

    global = SUB_AST(0);
    globals = SUB_AST(1);

    walk_global(global, names, syms);
    walk_globals(globals, names, syms);
}

void walk_global(AST *ast, const char **names, Symbols *syms) {
    (void)syms;

    if (IS_NAME("global_decl")) {
        ast = SUB_AST(0);
        (void)walk_decl(ast, names, syms);
        /* TODO: assert declaration's expression is KACT */
    } else if (IS_NAME("global_func")) {
        ast = SUB_AST(0);
        assert(IS_NAME("function"));
        walk_function(ast, names, syms);
    } else if (IS_NAME("global_struct")) {
        ObjStruct *x;
        Declaration decl;

        ast = SUB_AST(0);
        assert(IS_NAME("struct"));
        x = malloc(sizeof(ObjStruct));
        *x = walk_struct(ast, names, syms);

        decl.lineno = x->lineno;
        decl.type.id = TYPE_STRUCT_DEF;
        decl.type.data = x;
        decl.mut = false;
        PUSH_TO_SCOPE(x->name, decl);
    } else if (IS_NAME("global_impl")) {
        ast = SUB_AST(0);
        assert(IS_NAME("impl"));
        walk_impl(ast, names, syms);
    } else
        UNREACHABLE;
}
