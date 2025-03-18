#include "decl.h"
#include "../../expr/expr.h"
#include <tokens.h>

Declaration walk_decl(AST *ast, const char **names, Symbols *syms) {
    bool typed = false;
    AST *lhs, *rhs;
    char *name = NULL;
    ObjExpression expr;
    Declaration decl;

    /* Check mutability and get lhs and rhs */
    if (IS_NAME("decl_let_id")) {
        decl.mut = false;
        typed = false;
    } else if (IS_NAME("decl_let_p_id")) {
        decl.mut = true;
        typed = false;
    } else if (IS_NAME("decl_let_typed")) {
        decl.mut = false;
        typed = true;
    } else if (IS_NAME("decl_let_p_typed")) {
        decl.mut = true;
        typed = true;
    } else
        UNREACHABLE;

    if (typed)
        todo();

    /* Assumed untyped */
    if (!decl.mut) {
        lhs = SUB_AST(1);
        rhs = SUB_AST(3);
    } else {
        lhs = SUB_AST(2);
        rhs = SUB_AST(4);
    }

    do {
        Capture *id = (Capture *)lhs;
        assert(id->token == T_ID);
        name = (char *)(id->info);
        decl.lineno = id->lineno;
    } while (0);

    /* Get rhs */
    expr = walk_expr(rhs, names, syms);

    decl.type = expr.type;
    PUSH_TO_SCOPE(name, decl);
    return decl;
}
