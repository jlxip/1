#include "walk.h"
#include <common.h>
#include <compiler.h>
#include <stdio.h>
#include <string.h>

WALKFUNC(uses);
WALKFUNC(globals);
WALKFUNC(global);
WALKFUNC(global_assign);

void walk(ASTRoot root) {
    AST *ast = &root.ast;
    char **names = root.names;
    Symbols syms = NULL;

    assert(IS_NAME("program"));

    /* Initialize syms and create global scope */
    buffer_new(&syms, sizeof(map));
    NEW_SCOPE;

    walk_uses(SUB_AST(0), (const char **)names, &syms);
    walk_globals(SUB_AST(1), (const char **)names, &syms);
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
    /*AST *sub;*/

    if (IS_NAME("global_assign")) {
        AST *assignment = SUB_AST(0);
        walk_global_assign(assignment, names, syms);
    } else if (IS_NAME("global_func")) {
        todo();
    } else if (IS_NAME("global_struct")) {
        todo();
    } else
        UNREACHABLE;
}

void walk_global_assign(AST *ast, const char **names, Symbols *syms) {
    /* This is different to a regular assignment because rhs is expected to be
     * known at compile time */
    (void)syms;

    if (IS_NAME("assign_eq")) {
        todo();
    } else if (IS_NAME("assign_plus"))
        todo();
    else if (IS_NAME("assign_minus"))
        todo();
    else if (IS_NAME("assign_star"))
        todo();
    else if (IS_NAME("assign_slash"))
        todo();
    else if (IS_NAME("assign_hat"))
        todo();
    else if (IS_NAME("assign_amp"))
        todo();
    else if (IS_NAME("assign_bar"))
        todo();
    else
        todo();
}
