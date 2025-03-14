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
    AST *lhs;
    char *name = NULL;
    size_t lineno = 0;
    ObjExpression expr;
    Declaration decl;

    /* Check mutability and get assignment */
    if (IS_NAME("decl_let")) {
        ast = SUB_AST(1);
    } else if (IS_NAME("decl_letp")) {
        mut = true;
        ast = SUB_AST(2);
    } else
        UNREACHABLE;

    if (!IS_NAME("assign_eq"))
        throw("only `=' can be used in global declarations");

    /* Get lhs */
    lhs = SUB_AST(0);
    do {
        AST *ast = lhs;
        if (IS_NAME("lhs_typed_rec") || IS_NAME("lhs_id_rec")) {
            /* There's no reason for this but I'm lazy */
            throw("only one declaration per line in global scope");
        }

        if (IS_NAME("lhs_typed")) {
            todo();
        } else if (IS_NAME("lhs_id")) {
            Capture *id = (Capture *)SUB_AST(0);
            assert(id->token == T_ID);
            name = (char *)(id->info);
            lineno = id->lineno;
        }
    } while (0);

    /* Get rhs */
    ast = SUB_AST(2);
    if (IS_NAME("rhs_rec")) {
        /* Same reason as above (none) */
        throw("only one right-hand side per line in global scope");
    }

    assert(IS_NAME("rhs_direct"));
    ast = SUB_AST(0);

    expr = walk_expr(ast, names, syms);
    decl.lineno = lineno;
    decl.type = expr.type;
    decl.mut = mut;
    PUSH_TO_SCOPE(name, decl);
}
