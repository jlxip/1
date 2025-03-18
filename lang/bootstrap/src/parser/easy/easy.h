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
    void *data;
} TokenData;

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

ASTRoot grammar_parse(void *g, const TokenData *stream);

#endif
