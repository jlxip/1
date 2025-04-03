#include "decl.h"
#include "../../expr/expr.h"

Declaration walk_decl(WalkCtx *ctx, AST *ast) {
    bool typed = false;
    AST *lhs, *rhs;
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
        lhs = SUB_AST(1);
        rhs = SUB_AST(3);
    } else {
        lhs = SUB_AST(2);
        rhs = SUB_AST(4);
    }

    do {
        Token *id = (Token *)lhs;
        assert(id->id == T_ID);
        ret.name = id->data.str;
        ret.lineno = id->lineno;
    } while (0);

    /* Get rhs */
    expr = walk_expr(ctx, rhs);

    ret.type = expr.type;
    PUSH_TO_SCOPE(ret.name, ret);
    return ret;
}
