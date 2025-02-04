#include "sdt.h"
#include <common.h>
#include <string.h>
#include <tokens.h>

/* CONDITIONAL -> IF */
void *sdt_cond_if(void **sub) { NOT_IMPLEMENTED; }

/* CONDITIONAL -> SWITCH */
void *sdt_cond_switch(void **sub) { NOT_IMPLEMENTED; }

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

/* SWITCH -> switch EXPRESSION obraces SWITCH_BODY cbraces */
void *sdt_switch(void **sub) { NOT_IMPLEMENTED; }

/* SWITCH_BODY -> SWITCH_CASE SWITCH_BODY */
void *sdt_switch_rec(void **sub) { NOT_IMPLEMENTED; }

/* SWITCH_BODY -> SWITCH_CASE */
void *sdt_switch_one(void **sub) { NOT_IMPLEMENTED; }

/* SWITCH_CASE -> case LITERAL colon STATEMENTS */
void *sdt_switch_case(void **sub) { NOT_IMPLEMENTED; }

/* SWITCH_CASE -> default colon STATEMENTS */
void *sdt_switch_default(void **sub) { NOT_IMPLEMENTED; }
