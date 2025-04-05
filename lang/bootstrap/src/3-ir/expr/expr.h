#ifndef WALK_EXPR_H
#define WALK_EXPR_H

#include "../walk.h"

typedef struct {
    TokenIdx mst;
    Type type;
    /* string code; */
    /* bool kact; */
    /* void* kact_val; */
} ObjExpression;

ObjExpression walk_expr(WalkCtx *ctx, AST *ast);

#endif
