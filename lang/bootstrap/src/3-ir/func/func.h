#ifndef WALK_FUNC_H
#define WALK_FUNC_H

#include "../walk.h"

typedef struct {
    TokenIdx name;
    buffer params; /* buffer<Declaration*> */
    Type ret;
    /* string code; */
    /* bool kact; */
} ObjFunction;

ObjFunction walk_function(WalkCtx *ctx, AST *ast);
buffer walk_params(WalkCtx *ctx, AST *ast);

#endif
