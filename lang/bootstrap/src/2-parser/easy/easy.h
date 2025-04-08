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

#ifndef IR_TYPEDEF
#define IR_TYPEDEF
/* We're going full data-oriented design for this one */
typedef struct {
    /* Types and data of children */
    buffer types; /* buffer<size_t (IRType)> */
    buffer ids;   /* buffer<size_t (iToken/iIR)> */
} IR;
typedef size_t iIR;
typedef buffer IRs; /* buffer<IR> */
#endif

IRs grammar_parse(void *g, const TokenData *stream);

#endif
