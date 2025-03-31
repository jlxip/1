#include "block.h"
#include "../expr/expr.h"
#include "ctrl/ctrl.h"
#include "decl/decl.h"
#include <tokens.h>

void walk_statements(AST *ast, const char **names, Symbols *syms);
void walk_statement(AST *ast, const char **names, Symbols *syms);

ObjBlock walk_block(AST *ast, const char **names, Symbols *syms) {
    ObjBlock ret;

    assert(IS_NAME("block"));
    ret.lineno = ((Token *)SUB_AST(0))->lineno;

    ast = SUB_AST(1);
    walk_statements(ast, names, syms);

    return ret;
}

void walk_statements(AST *ast, const char **names, Symbols *syms) {
    if (IS_NAME("stmts_null"))
        return;
    assert(IS_NAME("stmts_rec"));

    walk_statement(SUB_AST(0), names, syms);
    walk_statements(SUB_AST(2), names, syms);
}

void walk_statement(AST *ast, const char **names, Symbols *syms) {
    (void)syms;

    if (IS_NAME("stmt_block")) {
        todo();
    } else if (IS_NAME("stmt_decl")) {
        ast = SUB_AST(0);
        (void)walk_decl(ast, names, syms);
    } else if (IS_NAME("stmt_expr")) {
        ast = SUB_AST(0);
        (void)walk_expr(ast, names, syms);
    } else if (IS_NAME("stmt_ctrl")) {
        ast = SUB_AST(0);
        walk_ctrl(ast, names, syms);
    } else if (IS_NAME("stmt_cond")) {
        todo();
    } else if (IS_NAME("stmt_loop")) {
        todo();
    } else
        UNREACHABLE;
}
