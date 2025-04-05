#ifndef WALK_ANNOTATIONS_H
#define WALK_ANNOTATIONS_H

#include "../walk.h"

typedef struct {
    TokenIdx name;
    buffer args; /* buffer<void*> */
} ObjAnnotation;

typedef map ObjAnnotations; /* map<char*, ObjAnnotation> */

ObjAnnotations walk_annotations(WalkCtx *ctx, AST *ast);

#endif
