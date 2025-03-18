#ifndef WALK_PRIMARY_H
#define WALK_PRIMARY_H

#include "../../walk.h"

typedef struct {
    size_t lineno;
    const char *name;
    /* string code; */
} ObjPrimary;

ObjPrimary walk_primary(AST *ast, const char **names, Symbols *syms);

#endif
