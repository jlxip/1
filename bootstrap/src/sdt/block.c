#include "sdt.h"
#include <common.h>
#include <string.h>
#include <tokens.h>

/* BLOCK -> obraces STATEMENTS cbraces */
void *sdt_block(void **sub) { NOT_IMPLEMENTED; }

/* STATEMENTS -> STATEMENT semicolon STATEMENTS */
void *sdt_stmts_rec(void **sub) { NOT_IMPLEMENTED; }

/* STATEMENTS -> EPSILON */
void *sdt_stmts_null(void **sub) { NOT_IMPLEMENTED; }

/* STATEMENT -> BLOCK */
void *sdt_stmt_block(void **sub) { NOT_IMPLEMENTED; }

/* STATEMENT -> EXPRESSION */
void *sdt_stmt_expr(void **sub) { NOT_IMPLEMENTED; }

/* STATEMENT -> CONTROL */
void *sdt_stmt_ctrl(void **sub) { NOT_IMPLEMENTED; }

/* STATEMENT -> CONDITIONAL */
void *sdt_stmt_cond(void **sub) { NOT_IMPLEMENTED; }

/* STATEMENT -> LOOP */
void *sdt_stmt_loop(void **sub) { NOT_IMPLEMENTED; }
