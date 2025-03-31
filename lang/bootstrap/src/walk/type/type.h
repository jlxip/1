#ifndef WALK_TYPE_H
#define WALK_TYPE_H

#include "../walk.h"

/* Can't move Type definition here due to circular includes */

typedef struct {
    size_t lineno;
    buffer fields; /* buffer<Type> */
    size_t rep;
} ObjTupleDef;

Type walk_type(AST *ast, const char **names, Symbols *syms);

#endif
