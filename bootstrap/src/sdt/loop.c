#include "sdt.h"
#include <common.h>
#include <string.h>
#include <tokens.h>

/* LOOP -> FOR */
void *sdt_loop_for(void **sub) { NOT_IMPLEMENTED; }

/* LOOP -> WHILE */
void *sdt_loop_while(void **sub) { NOT_IMPLEMENTED; }

/* FOR -> for id in EXPRESSION STATEMENT */
void *sdt_for(void **sub) { NOT_IMPLEMENTED; }

/* WHILE -> while EXPRESSION STATEMENT */
void *sdt_while(void **sub) { NOT_IMPLEMENTED; }
