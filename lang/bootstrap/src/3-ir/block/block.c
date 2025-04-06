#include "block.h"
#include "../expr/expr.h"
#include "decl/decl.h"

static void walk_statements(WalkCtx *ctx, AST *ast);
static void walk_statement(WalkCtx *ctx, AST *ast);

ObjBlock walk_block(WalkCtx *ctx, AST *ast) {
    ObjBlock ret;

    assert(IS_NAME("block"));
    ret.mst = SUB(0);

    ast = AST(SUB(1));
    walk_statements(ctx, ast);

    return ret;
}

static void walk_statements(WalkCtx *ctx, AST *ast) {
    if (IS_NAME("stmts_null"))
        return;
    assert(IS_NAME("stmts_rec"));

    walk_statement(ctx, AST(SUB(0)));
    walk_statements(ctx, AST(SUB(2)));
}

static void walk_statement(WalkCtx *ctx, AST *ast) {
    if (IS_NAME("stmt_block")) {
        todo();
    } else if (IS_NAME("stmt_decl")) {
        ast = AST(SUB(0));
        (void)walk_decl(ctx, ast);
    } else if (IS_NAME("stmt_expr")) {
        ast = AST(SUB(0));
        (void)walk_expr(ctx, ast);
        /*} else if (IS_NAME("stmt_ctrl")) {
        ast = SUB_AST(0);
        walk_ctrl(ctx, ast);*/
    } else if (IS_NAME("stmt_cond")) {
        todo();
    } else if (IS_NAME("stmt_loop")) {
        todo();
    } else
        UNREACHABLE;
}
