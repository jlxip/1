#include "walk.h"
#include "expr/expr.h"
#include "func/func.h"
#include <stdio.h>
#include <tokens.h>

void walk_uses(AST *ast, const char **names, Symbols *syms);
void walk_globals(AST *ast, const char **names, Symbols *syms);
void walk_global(AST *ast, const char **names, Symbols *syms);
void walk_global_decl(AST *ast, const char **names, Symbols *syms);

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
        walk_global_decl(ast, names, syms);
    } else if (IS_NAME("global_func")) {
        ast = SUB_AST(0);
        assert(IS_NAME("function"));
        walk_function(ast, names, syms);
    } else if (IS_NAME("global_struct")) {
        todo();
    } else
        UNREACHABLE;
}

void walk_global_decl(AST *ast, const char **names, Symbols *syms) {
    bool mut = false;
    bool typed = false;
    AST *lhs, *rhs;
    char *name = NULL;
    size_t lineno = 0;
    ObjExpression expr;
    Declaration decl;

    /* Check mutability and get lhs and rhs */
    if (IS_NAME("decl_let_id")) {
        mut = false;
        typed = false;
    } else if (IS_NAME("decl_let_p_id")) {
        mut = true;
        typed = false;
    } else if (IS_NAME("decl_let_typed")) {
        mut = false;
        typed = true;
    } else if (IS_NAME("decl_let_p_typed")) {
        mut = true;
        typed = true;
    } else
        UNREACHABLE;

    if (typed)
        todo();

    /* Assumed untyped */
    if (!mut) {
        lhs = SUB_AST(1);
        rhs = SUB_AST(3);
    } else {
        lhs = SUB_AST(2);
        rhs = SUB_AST(4);
    }

    do {
        Capture *id = (Capture *)lhs;
        assert(id->token == T_ID);
        name = (char *)(id->info);
        lineno = id->lineno;
    } while (0);

    /* Get rhs */
    expr = walk_expr(rhs, names, syms);

    decl.lineno = lineno;
    decl.type = expr.type;
    decl.mut = mut;
    PUSH_TO_SCOPE(name, decl);
}
