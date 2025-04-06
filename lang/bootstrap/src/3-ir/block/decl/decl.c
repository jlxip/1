#include "decl.h"
#include "../../expr/expr.h"

Declaration walk_decl(WalkCtx *ctx, AST *ast) {
    bool typed = false;
    AST *rhs;
    ObjExpression expr;
    Declaration ret;

    /* Check mutability and get lhs and rhs */
    if (IS_NAME("decl_let_id")) {
        ret.mut = false;
        typed = false;
    } else if (IS_NAME("decl_let_p_id")) {
        ret.mut = true;
        typed = false;
    } else if (IS_NAME("decl_let_typed")) {
        ret.mut = false;
        typed = true;
    } else if (IS_NAME("decl_let_p_typed")) {
        ret.mut = true;
        typed = true;
    } else
        UNREACHABLE;

    if (typed)
        todo();

    /* Assumed untyped */
    if (!ret.mut) {
        ret.name = SUB(1);
        rhs = AST(SUB(3));
    } else {
        ret.name = SUB(2);
        rhs = AST(SUB(4));
    }

    /* Get rhs */
    expr = walk_expr(ctx, rhs);

    ret.type = expr.type;
    PUSH_TO_SCOPE(TOKEN(ret.name)->data.str, ret);
    return ret;
}
