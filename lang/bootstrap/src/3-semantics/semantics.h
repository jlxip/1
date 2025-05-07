#ifndef SEMANTICS_H
#define SEMANTICS_H

#include <common.h>
#include <compiler.h>
#include <ds/map.h>
#include <ir.h>
#include <types.h>

typedef map Symbols;        /* map<const char*, iIR> */
typedef buffer SymbolTable; /* buffer<Symbols> */

typedef struct {
    /* --- GIVEN --- */
    Tokens tokens;
    IRs irs;

    /* --- AUGMENTED --- */
    Types types;
    buffer mangling; /* buffer<char*> */

    /* --- STATE --- */
    /* Symbol tables */
    buffer tables; /* buffer<SymbolTable> */
    /* Module stack, e.g. ["std", "ds", "vec"] */
    buffer modstack; /* buffer<const char*> */
    /* Expected return type of called functions */
    buffer funcrets; /* buffer<Type> */
    /* Current symbol table */
    iIR current;
} Ctx;

char *mangle(Ctx *ctx, const char *name);

#endif
