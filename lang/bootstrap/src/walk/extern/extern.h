#ifndef WALK_EXTERN_H
#define WALK_EXTERN_H

#include "../walk.h"

typedef struct {
    size_t lineno;
    const char *name;
    buffer params; /* buffer<Declaration*> */
    Type ret;
} ObjExtern;

ObjExtern walk_extern(AST *ast, const char **names, Symbols *syms);

#endif
