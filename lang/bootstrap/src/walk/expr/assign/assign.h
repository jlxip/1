#ifndef WALK_ASSIGN_H
#define WALK_ASSIGN_H

#include "../../walk.h"

typedef struct {
    TokenIdx mst;
    Type type;
    /* string code; */
} ObjAssign;

ObjAssign walk_assign(WalkCtx *ctx, AST *ast);

#endif
