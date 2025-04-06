#ifndef EASY_H
#define EASY_H

#include <ds/buffer.h>
#include <stddef.h>

void *grammar(
    const char **tokens, const char **nts, const char *cstr, const char *start);
void grammar_compile(void *g);
void grammar_out(void *g);

typedef struct {
    size_t sym;
    size_t data;
} TokenData;

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

ASTPack grammar_parse(void *g, const TokenData *stream);

#endif
