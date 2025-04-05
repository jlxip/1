#ifndef WALK_STRUCT_INST
#define WALK_STRUCT_INST

#include "../../walk.h"

typedef struct {
    iToken mst;
    Type type;
    map fills; /* map<char*, ObjExpression> */
} ObjStructInst;

ObjStructInst walk_struct_inst(WalkCtx *ctx, AST *ast);

#endif
