#include "assign.h"
#include "../../lookup/lookup.h"
#include "../expr.h"

ObjAssign walk_assign(WalkCtx *ctx, AST *ast) {
    ObjAssign ret;
    Declaration *decl;
    ObjExpression rhs;

    decl = lookup(ctx, SUB_AST(0));
    if (!decl->mut)
        throwe("tried to assign to immutable symbol: %s",
            TOKEN(decl->name)->data.str);

    ret.mst = (TokenIdx)SUB_AST(1);

    rhs = walk_expr(ctx, SUB_AST(2));
    if (decl->type.id != rhs.type.id)
        throwe("type error in assignment to: %s", TOKEN(decl->name)->data.str);

    switch (decl->type.id) {
    case TYPE_BOOL:
    case TYPE_WORD:
    case TYPE_FLOAT:
    case TYPE_STRING:
        break;
    default:
        UNREACHABLE;
    }

    /*if (IS_NAME("assign_eq")) {
        todo();
    } else if (IS_NAME("assign_plus")) {
        todo();
    } else if (IS_NAME("assign_minus")) {
        todo();
    } else if (IS_NAME("assign_star")) {
        todo();
    } else if (IS_NAME("assign_slash")) {
        todo();
    } else if (IS_NAME("assign_hat")) {
        todo();
    } else if (IS_NAME("assign_amp")) {
        todo();
    } else if (IS_NAME("assign_bar")) {
        todo();
    } else
    UNREACHABLE;*/

    ret.type = rhs.type;
    return ret;
}
