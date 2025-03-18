#include "expr.h"
#include "assign/assign.h"
#include "lit/lit.h"

ObjExpression walk_expr(AST *ast, const char **names, Symbols *syms) {
    ObjExpression ret;

    if (IS_NAME("expr_par")) {
        todo();
    } else if (IS_NAME("expr_id")) {
        todo();
    } else if (IS_NAME("expr_lit")) {
        ObjLiteral lit;
        ast = SUB_AST(0);
        lit = walk_lit(ast, names, syms);
        ret.lineno = lit.lineno;
        ret.type = lit.type;
    } else if (IS_NAME("expr_hat")) {
        todo();
    } else if (IS_NAME("expr_amp")) {
        todo();
    } else if (IS_NAME("expr_bar")) {
        todo();
    } else if (IS_NAME("expr_star")) {
        todo();
    } else if (IS_NAME("expr_slash")) {
        todo();
    } else if (IS_NAME("expr_plus")) {
        todo();
    } else if (IS_NAME("expr_minus")) {
        todo();
    } else if (IS_NAME("expr_is")) {
        todo();
    } else if (IS_NAME("expr_deq")) {
        todo();
    } else if (IS_NAME("expr_neq")) {
        todo();
    } else if (IS_NAME("expr_lt")) {
        todo();
    } else if (IS_NAME("expr_leq")) {
        todo();
    } else if (IS_NAME("expr_gt")) {
        todo();
    } else if (IS_NAME("expr_geq")) {
        todo();
    } else if (IS_NAME("expr_not")) {
        todo();
    } else if (IS_NAME("expr_and")) {
        todo();
    } else if (IS_NAME("expr_and2")) {
        todo();
    } else if (IS_NAME("expr_or")) {
        todo();
    } else if (IS_NAME("expr_or2")) {
        todo();
    } else if (IS_NAME("expr_assign")) {
        ObjAssign assign;
        ast = SUB_AST(0);
        assign = walk_assign(ast, names, syms);
        ret.lineno = assign.lineno;
        ret.type = assign.type;
    } else
        UNREACHABLE;

    return ret;
}
