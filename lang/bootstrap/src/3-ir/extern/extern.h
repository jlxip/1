#ifndef WALK_EXTERN_H
#define WALK_EXTERN_H

#include "../walk.h"

typedef struct {
    iToken name;
    buffer params; /* buffer<Declaration*> */
    Type ret;
} ObjExtern;

ObjExtern walk_extern(WalkCtx *ctx, AST *ast);

#endif
