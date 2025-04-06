#include "expr.h"
#include "assign/assign.h"
#include "lit/lit.h"
#include "struct_inst/struct_inst.h"

ObjExpression walk_expr(WalkCtx *ctx, AST *ast) {
    ObjExpression ret;

    if (IS_NAME("expr_par")) {
        todo();
    } else if (IS_NAME("expr_primary")) {
        todo();
    } else if (IS_NAME("expr_lit")) {
        ObjLiteral lit;
        ast = AST(SUB(0));
        lit = walk_lit(ctx, ast);
        ret.mst = lit.mst;
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
    } else if (IS_NAME("expr_struct_inst")) {
        ObjStructInst inst;
        ast = AST(SUB(0));
        inst = walk_struct_inst(ctx, ast);
        ret.mst = inst.mst;
        ret.type = inst.type;
        /* inst.fills ignored for now */
    } else if (IS_NAME("expr_assign")) {
        ObjAssign assign;
        ast = AST(SUB(0));
        assign = walk_assign(ctx, ast);
        ret.mst = assign.mst;
        ret.type = assign.type;
    } else
        UNREACHABLE;

    return ret;
}
