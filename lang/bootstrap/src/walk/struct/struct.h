#ifndef WALK_STRUCT_H
#define WALK_STRUCT_H

#include "../walk.h"

typedef struct {
    size_t lineno;
    const char *name; /* This will change to char* */
    map fields;       /* map<const char*, Type> */
    buffer generic;   /* buffer<const char*> */
} ObjStruct;

typedef struct {
    Declaration *struct_def;
    buffer specific; /* buffer<Type> */
} ObjSpecificStruct;

ObjStruct walk_struct(WalkCtx *ctx, AST *ast);

#endif
