#include "walk.h"
#include "block/decl/decl.h"
#include "expr/expr.h"
#include "extern/extern.h"
#include "func/func.h"
#include "impl/impl.h"
#include "struct/struct.h"
#include <stdio.h>

static void walk_uses(WalkCtx *ctx, AST *ast);
static void walk_globals(WalkCtx *ctx, AST *ast);
static void walk_global(WalkCtx *ctx, AST *ast);

void walk(ASTRoot root, Tokens tokens) {
    AST *ast = &root.ast;
    WalkCtx _ctx, *ctx;

    _ctx.tokens = tokens;
    _ctx.names = (const char **)root.names;
    _ctx.syms = NULL;
    ctx = &_ctx;

    assert(IS_NAME("program"));

    /* Initialize syms and create global scope */
    buffer_new(&_ctx.syms, sizeof(map));
    NEW_SCOPE;

    walk_uses(ctx, SUB_AST(0));
    walk_globals(ctx, SUB_AST(1));
}

static void walk_uses(WalkCtx *ctx, AST *ast) {
    if (!IS_NAME("uses_null"))
        todo();
}

static void walk_globals(WalkCtx *ctx, AST *ast) {
    AST *global, *globals;

    if (IS_NAME("globals_null"))
        return;
    assert(IS_NAME("globals_rec"));

    global = SUB_AST(0);
    globals = SUB_AST(1);

    walk_global(ctx, global);
    walk_globals(ctx, globals);
}

static void walk_global(WalkCtx *ctx, AST *ast) {
    if (IS_NAME("global_decl")) {
        ast = SUB_AST(0);
        (void)walk_decl(ctx, ast);
        /* TODO: assert declaration's expression is KACT */
    } else if (IS_NAME("global_func")) {
        ast = SUB_AST(0);
        walk_function(ctx, ast);
    } else if (IS_NAME("global_struct")) {
        ObjStruct *x;
        Declaration decl;

        ast = SUB_AST(0);
        assert(IS_NAME("struct"));
        x = malloc(sizeof(ObjStruct));
        *x = walk_struct(ctx, ast);

        decl.lineno = x->lineno;
        decl.name = x->name;
        decl.type.id = TYPE_STRUCT_DEF;
        decl.type.data = x;
        decl.mut = false;
        PUSH_TO_SCOPE(x->name, decl);
    } else if (IS_NAME("global_impl")) {
        ast = SUB_AST(0);
        assert(IS_NAME("impl"));
        walk_impl(ctx, ast);
    } else if (IS_NAME("global_extern")) {
        ast = SUB_AST(0);
        walk_extern(ctx, ast);
    } else
        UNREACHABLE;
}
