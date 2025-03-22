#include "assign.h"
#include "../expr.h"
#include "../primary/primary.h"

ObjAssign walk_assign(AST *ast, const char **names, Symbols *syms) {
    ObjAssign ret;
    ObjPrimary lhs;
    Declaration *decl;
    ObjExpression rhs;

    lhs = walk_primary(SUB_AST(0), names, syms);
    LOOKUP(decl, lhs.name);
    if (!decl->mut)
        throwe("tried to assign to immutable symbol: %s", lhs.name);

    rhs = walk_expr(SUB_AST(2), names, syms);
    if (decl->type.id != rhs.type.id)
        throwe("type error in assignment to: %s", lhs.name);

    switch (decl->type.id) {
    case TYPE_BOOL:
    case TYPE_WORD:
    case TYPE_FLOAT:
    case TYPE_STRING:
        break;
    default:
        todo();
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

    ret.lineno = lhs.lineno;
    ret.type = rhs.type;
    return ret;
}
