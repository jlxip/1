#ifndef WALK_BLOCK_H
#define WALK_BLOCK_H

#include "../walk.h"

typedef struct {
    size_t lineno;
    /* string code; */
} ObjBlock;

ObjBlock walk_block(WalkCtx *ctx, AST *ast);

#endif
