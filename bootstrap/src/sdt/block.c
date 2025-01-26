#include "sdt.h"
#include <common.h>
#include <string.h>
#include <tokens.h>

/* BLOCK -> obraces STATEMENTS cbraces */
void *sdt_block(void **sub) { NOT_IMPLEMENTED; }

/* STATEMENTS -> STATEMENT semicolon STATEMENTS */
void *sdt_statements_rec(void **sub) { NOT_IMPLEMENTED; }

/* STATEMENTS -> STATEMENT semicolon */
void *sdt_statements_direct(void **sub) { NOT_IMPLEMENTED; }

/* STATEMENT -> EXPRESSION */
void *sdt_statement_expr(void **sub) { NOT_IMPLEMENTED; }
