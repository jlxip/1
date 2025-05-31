#ifndef SEMANTICS_H
#define SEMANTICS_H

#include <common.h>
#include <compiler.h>
#include <ds/map.h>
#include <ir.h>
#include <types.h>

typedef map Symbols;        /* map<const char*, iIR> */
typedef buffer SymbolTable; /* buffer<Symbols> */

typedef map ShadowingTable; /* map<const char*, size_t> */

typedef struct {
    /* --- GIVEN --- */
    Tokens tokens;
    IRs irs;

    /* --- AUGMENTED --- */
    Types types;
    buffer mangling; /* buffer<char*> */

    /* --- STATE --- */
    /* Raw names, prior to mangling */
    buffer rawnames; /* buffer<char*> */
    /* Symbol tables */
    buffer tables; /* buffer<SymbolTable> */
    /* Current symbol table */
    iIR current;
    /* Mangling stack, e.g. ["std", "ds", "vec", "Vec"] */
    buffer mangling_stack; /* buffer<const char*> */
    /* Expected return type of called functions */
    buffer funcrets; /* buffer<Type> */
    /* Shadowing stack */
    buffer shadowing; /* buffer<ShadowingTable> */
    /* Mutable context (for impl @mut fn) */
    bool mut_context;
} Ctx;

char *mangle(Ctx *ctx, const char *name);
char *mangle_var(Ctx *ctx, const char *name);

void push_scope(Ctx *ctx);
void pop_scope(Ctx *ctx);
void push_to_scope(Ctx *ctx, const char *name, iIR iir);
bool in_scope(Ctx *ctx, const char *name);

#endif
