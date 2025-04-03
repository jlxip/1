#ifndef WALK_LIT_H
#define WALK_LIT_H

#include "../expr.h"

typedef struct {
    size_t lineno;
    Type type;
    /*string code;*/
    /* bool kact; */
    /* void* kact_val; */
} ObjLiteral;

ObjLiteral walk_lit(WalkCtx *ctx, AST *ast);

#endif
