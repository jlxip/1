#include "sdt.h"
#include <common.h>
#include <string.h>
#include <tokens.h>

/* CONDITIONAL -> IF */
void *sdt_cond_if(void **sub) { NOT_IMPLEMENTED; }

/* IF -> if EXPRESSION STATEMENT */
void *sdt_if_only(void **sub) { NOT_IMPLEMENTED; }

/* IF -> if EXPRESSION STATEMENT ELIF */
void *sdt_if_elif(void **sub) { NOT_IMPLEMENTED; }

/* ELIF -> elif EXPRESSION STATEMENT */
void *sdt_elif_only(void **sub) { NOT_IMPLEMENTED; }

/* ELIF -> elif EXPRESSION STATEMENT ELIF */
void *sdt_elif_rec(void **sub) { NOT_IMPLEMENTED; }

/* ELIF -> ELSE */
void *sdt_elif_else(void **sub) { NOT_IMPLEMENTED; }

/* ELSE -> else STATEMENT */
void *sdt_else(void **sub) { NOT_IMPLEMENTED; }
