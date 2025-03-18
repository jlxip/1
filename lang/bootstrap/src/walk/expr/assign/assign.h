#ifndef WALK_ASSIGN_H
#define WALK_ASSIGN_H

#include "../../walk.h"

typedef struct {
    size_t lineno;
    Type type;
    /* string code; */
} ObjAssign;

ObjAssign walk_assign(AST *ast, const char **names, Symbols *syms);

#endif
