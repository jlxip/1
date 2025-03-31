#ifndef COMPILER_H
#define COMPILER_H

#include <tokens.h>

Tokens get_tokens(const char *code);

#ifndef AST_TYPEDEF
#define AST_TYPEDEF
typedef struct {
    size_t prod;
    buffer sub; /* buffer<void*> */
} AST;
#endif

#ifndef ASTROOT_TYPEDEF
#define ASTROOT_TYPEDEF
typedef struct {
    char **names;
    AST ast;
} ASTRoot;
#endif

ASTRoot parse(const Tokens tokens);
void walk(ASTRoot ast);

#endif
