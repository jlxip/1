#ifndef WALK_ANNOTATIONS_H
#define WALK_ANNOTATIONS_H

#include "../walk.h"

typedef struct {
    size_t lineno;
    const char *name;
    buffer args; /* buffer<ObjPrimary> */
} ObjAnnotation;

typedef buffer ObjAnnotations; /* buffer<ObjAnnotation> */

ObjAnnotations walk_annotations(AST *ast, const char **names, Symbols *syms);

#endif
