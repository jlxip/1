#ifndef EMIT_H
#define EMIT_H

#include <common.h>
#include <compiler.h>
#include <ds/map.h>
#include <ds/string.h>
#include <ir.h>

typedef struct {
    Tokens tokens;
    IRs irs;
    SemResult sem;

    buffer decl; /* buffer<buffer<string>> */
    map decln;   /* map<string, size_t> */

    iIR self_struct; /* iIR of Self in current impl */
} Ctx;

#define GET_IR(N) (buffer_get(ctx->irs, N, IR))
#define GET_IRTYPE(N) (*buffer_get(ir->types, N, IRType))
#define GET_IRID(N) (*buffer_get(ir->ids, N, iIR))
#define GET_TOKEN(N) (buffer_get(ctx->tokens, N, Token))

/* Returned by expressions */
typedef struct {
    string code;
    buffer above;    /* code above (auxiliary assignments) */
    string self_val; /* mangled name of instance for methods */
} Expr;

#endif
