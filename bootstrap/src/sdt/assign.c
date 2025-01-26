#include "sdt.h"
#include <common.h>
#include <string.h>
#include <tokens.h>

/* ASSIGNMENT -> LHS eq RHS */
void *sdt_assign_eq(void **sub) { NOT_IMPLEMENTED; }

/* ASSIGNMENT -> LHS pluseq RHS */
void *sdt_assign_plus(void **sub) { NOT_IMPLEMENTED; }

/* ASSIGNMENT -> LHS minuseq RHS */
void *sdt_assign_minus(void **sub) { NOT_IMPLEMENTED; }

/* ASSIGNMENT -> LHS stareq RHS */
void *sdt_assign_star(void **sub) { NOT_IMPLEMENTED; }

/* ASSIGNMENT -> LHS slasheq RHS */
void *sdt_assign_slash(void **sub) { NOT_IMPLEMENTED; }

/* ASSIGNMENT -> LHS hateq RHS */
void *sdt_assign_hat(void **sub) { NOT_IMPLEMENTED; }

/* ASSIGNMENT -> LHS ampeq RHS */
void *sdt_assign_amp(void **sub) { NOT_IMPLEMENTED; }

/* ASSIGNMENT -> LHS bareq RHS */
void *sdt_assign_bar(void **sub) { NOT_IMPLEMENTED; }

/* LHS -> TYPED_ID comma LHS */
void *sdt_lhs_typed_rec(void **sub) { NOT_IMPLEMENTED; }

/* LHS -> TYPED_ID */
void *sdt_lhs_typed(void **sub) { NOT_IMPLEMENTED; }

/* LHS -> id comma LHS */
void *sdt_lhs_id_rec(void **sub) { NOT_IMPLEMENTED; }

/* LHS -> id */
void *sdt_lhs_id(void **sub) { NOT_IMPLEMENTED; }

/* TYPED_ID -> id colon TYPE */
void *sdt_typed(void **sub) { NOT_IMPLEMENTED; }

/* TYPE -> id lt TYPES gt */
void *sdt_type_typed(void **sub) { NOT_IMPLEMENTED; }

/* TYPE -> id */
void *sdt_type_direct(void **sub) { NOT_IMPLEMENTED; }

/* TYPES -> TYPE comma TYPES */
void *sdt_types_rec(void **sub) { NOT_IMPLEMENTED; }

/* TYPES -> TYPE */
void *sdt_types_direct(void **sub) { NOT_IMPLEMENTED; }

/* RHS -> EXPRESSION comma RHS */
void *sdt_rhs_rec(void **sub) { NOT_IMPLEMENTED; }

/* RHS -> EXPRESSION */
void *sdt_rhs_direct(void **sub) { NOT_IMPLEMENTED; }
