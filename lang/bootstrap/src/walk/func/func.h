#ifndef WALK_FUNC_H
#define WALK_FUNC_H

#include "../walk.h"

typedef struct {
    size_t lineno;
    char *name;
    buffer params; /* buffer<Declaration*> */
    Type ret;
    /* string code; */
    /* bool kact; */
} ObjFunction;

ObjFunction walk_function(AST *ast, const char **names, Symbols *syms);
buffer walk_params(AST *ast, const char **names, Symbols *syms);

#endif
