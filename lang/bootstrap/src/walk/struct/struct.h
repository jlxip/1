#ifndef WALK_STRUCT_H
#define WALK_STRUCT_H

#include "../walk.h"

typedef struct {
    size_t lineno;
    const char *name;    /* This will change to char* */
    map fields;          /* map<const char*, Type> */
    /*buffer generics;*/ /* buffer<const char*> */
} ObjStruct;

ObjStruct walk_struct(AST *ast, const char **names, Symbols *syms);

#endif
