#ifndef COMPILER_H
#define COMPILER_H

#include <tokens.h>
#include <types.h>

Tokens get_tokens(const char *code);

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

IRs parse(Tokens tokens);
Types semantics(Tokens tokens, IRs irs);
char *emit(Tokens tokens, IRs irs, Types types);

#endif
