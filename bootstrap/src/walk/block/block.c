#include "block.h"
#include "../expr/expr.h"
#include <tokens.h>

void walk_statements(AST *ast, const char **names, Symbols *syms);
void walk_statement(AST *ast, const char **names, Symbols *syms);

ObjBlock walk_block(AST *ast, const char **names, Symbols *syms) {
    ObjBlock ret;

    ast = SUB_AST(1);
    walk_statements(ast, names, syms);

    ret.lineno = ((Capture *)SUB_AST(0))->lineno;
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
        todo();
    } else if (IS_NAME("stmt_expr")) {
        ast = SUB_AST(0);
        (void)walk_expr(ast, names, syms);
    } else if (IS_NAME("stmt_ctrl")) {
        todo();
    } else if (IS_NAME("stmt_cond")) {
        todo();
    } else if (IS_NAME("stmt_loop")) {
        todo();
    } else
        UNREACHABLE;
}
