#ifndef COMPILER_H
#define COMPILER_H

#include <tokens.h>

Tokens get_tokens(const char *code);

#ifndef AST_TYPEDEF
#define AST_TYPEDEF
typedef struct {
    size_t prod;
    buffer sub; /* buffer<size_t (iToken/iAST)> */
} AST;
typedef size_t iAST;
typedef buffer ASTs; /* buffer<AST> */
#endif

#ifndef ASTPACK_TYPEDEF
#define ASTPACK_TYPEDEF
typedef struct {
    char **names;
    ASTs asts;
} ASTPack;
#endif

ASTPack parse(Tokens tokens);
void walk(ASTPack ast, Tokens tokens);

#endif
