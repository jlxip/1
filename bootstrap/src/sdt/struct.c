#include "sdt.h"
#include <common.h>
#include <string.h>
#include <tokens.h>

/* STRUCT -> struct id obraces STRUCT_DEF cbraces */
void *sdt_struct(void **sub) { NOT_IMPLEMENTED; }

/* STRUCT_DEF -> ASSIGNMENT semicolon STRUCT_DEF */
void *sdt_struct_def(void **sub) { NOT_IMPLEMENTED; }

/* STRUCT_DEF -> EPSILON */
void *sdt_struct_def_null(void **sub) { NOT_IMPLEMENTED; }
