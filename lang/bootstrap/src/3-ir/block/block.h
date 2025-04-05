#ifndef WALK_BLOCK_H
#define WALK_BLOCK_H

#include "../walk.h"

typedef struct {
    iToken mst;
    /* string code; */
} ObjBlock;

ObjBlock walk_block(WalkCtx *ctx, AST *ast);

#endif
