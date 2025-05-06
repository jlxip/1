#ifndef EMIT_H
#define EMIT_H

#include <common.h>
#include <compiler.h>
#include <ir.h>

typedef struct {
    Tokens tokens;
    IRs irs;
    Types types;
} Ctx;

#define GET_IR(N) (buffer_get(ctx->irs, N, IR))
#define GET_IRTYPE(N) (*buffer_get(ir->types, N, IRType))
#define GET_IRID(N) (*buffer_get(ir->ids, N, iIR))
#define GET_TOKEN(N) (buffer_get(ctx->tokens, N, Token))

#endif
