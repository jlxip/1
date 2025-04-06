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

void walk(ASTPack root, Tokens tokens) {
    WalkCtx _ctx, *ctx;
    AST *ast;

    _ctx.tokens = tokens;
    _ctx.names = (const char **)root.names;
    _ctx.asts = root.asts;
    _ctx.syms = NULL;
    ctx = &_ctx;

    ast = buffer_back(root.asts, AST);
    assert(IS_NAME("program"));

    /* Initialize syms and create global scope */
    buffer_new(&_ctx.syms, sizeof(map));
    NEW_SCOPE;

    walk_uses(ctx, AST(SUB(0)));
    walk_globals(ctx, AST(SUB(1)));
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

    global = AST(SUB(0));
    globals = AST(SUB(1));

    walk_global(ctx, global);
    walk_globals(ctx, globals);
}

static void walk_global(WalkCtx *ctx, AST *ast) {
    if (IS_NAME("global_decl")) {
        ast = AST(SUB(0));
        (void)walk_decl(ctx, ast);
        /* TODO: assert declaration's expression is KACT */
    } else if (IS_NAME("global_func")) {
        ast = AST(SUB(0));
        walk_function(ctx, ast);
    } else if (IS_NAME("global_struct")) {
        ObjStruct *x;
        Declaration decl;

        ast = AST(SUB(0));
        assert(IS_NAME("struct"));
        x = malloc(sizeof(ObjStruct));
        *x = walk_struct(ctx, ast);

        decl.name = x->name;
        decl.type.id = TYPE_STRUCT_DEF;
        decl.type.data = x;
        decl.mut = false;
        PUSH_TO_SCOPE(TOKEN(x->name)->data.str, decl);
    } else if (IS_NAME("global_impl")) {
        ast = AST(SUB(0));
        assert(IS_NAME("impl"));
        walk_impl(ctx, ast);
    } else if (IS_NAME("global_extern")) {
        ast = AST(SUB(0));
        walk_extern(ctx, ast);
    } else
        UNREACHABLE;
}
