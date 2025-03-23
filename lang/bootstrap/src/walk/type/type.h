#ifndef WALK_TYPE_H
#define WALK_TYPE_H

#include "../walk.h"

/* Can't move Type definition here due to circular includes */

Type walk_type(AST *ast, const char **names, Symbols *syms);

#endif
