#ifndef SEMANTICS_H
#define SEMANTICS_H

#include <common.h>
#include <compiler.h>
#include <ds/map.h>
#include <ir.h>
#include <types.h>

typedef struct {
    map args;
    Type ret;
} Function;

typedef struct {
    map functions; /* map<string, Function> */
} Module;

typedef struct {
    /* --- GIVEN --- */
    Tokens tokens;
    IRs irs;

    /* --- AUGMENTED --- */
    Types types;
    buffer mangling; /* buffer<char*> */

    /* --- STATE --- */
    /* The scope for this module */
    Module entry;
    /* Module stack, e.g. ["std", "ds", "vec"] */
    buffer modstack; /* buffer<const char*> */
    /* Which function am I in */
    Function *func;
} Ctx;

#define GET_IR(N) (buffer_get(ctx->irs, N, IR))
#define GET_IRTYPE(N) (*buffer_get(ir->types, N, IRType))
#define GET_IRID(N) (*buffer_get(ir->ids, N, iIR))
#define GET_TOKEN(N) (buffer_get(ctx->tokens, N, Token))

char *mangle(Ctx *ctx, const char *name);

#endif
