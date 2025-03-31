#ifndef WALK_STRUCT_INST
#define WALK_STRUCT_INST

#include "../../walk.h"

typedef struct {
    size_t lineno;
    Type type;
    map fills; /* map<char*, ObjExpression> */
} ObjStructInst;

ObjStructInst walk_struct_inst(AST *ast, const char **names, Symbols *syms);

#endif
