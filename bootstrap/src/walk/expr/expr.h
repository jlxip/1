#ifndef WALK_EXPR_H
#define WALK_EXPR_H

#include "../walk.h"

typedef struct {
    size_t lineno;
    Type type;
    /* string code; */
    /* bool kact; */
    /* void* kact_val; */
} ObjExpression;

ObjExpression walk_expr(AST *ast, const char **names, Symbols *syms);

#endif
