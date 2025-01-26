#include "sdt.h"
#include <common.h>
#include <string.h>
#include <tokens.h>

/* EXPRESSION -> opar EXPRESSION cpar */
void *sdt_expr_par(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> EXPRESSION hat EXPRESSION */
void *sdt_expr_hat(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> EXPRESSION amp EXPRESSION */
void *sdt_expr_amp(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> EXPRESSION bar EXPRESSION */
void *sdt_expr_bar(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> EXPRESSION star EXPRESSION */
void *sdt_expr_star(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> EXPRESSION slash EXPRESSION */
void *sdt_expr_slash(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> EXPRESSION plus EXPRESSION */
void *sdt_expr_plus(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> EXPRESSION minus EXPRESSION */
void *sdt_expr_minus(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> EXPRESSION is EXPRESSION */
void *sdt_expr_is(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> EXPRESSION deq EXPRESSION */
void *sdt_expr_deq(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> EXPRESSION neq EXPRESSION */
void *sdt_expr_neq(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> EXPRESSION lt EXPRESSION */
void *sdt_expr_lt(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> EXPRESSION leq EXPRESSION */
void *sdt_expr_leq(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> EXPRESSION gt EXPRESSION */
void *sdt_expr_gt(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> EXPRESSION geq EXPRESSION */
void *sdt_expr_geq(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> not EXPRESSION */
void *sdt_expr_not(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> EXPRESSION and EXPRESSION */
void *sdt_expr_and(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> EXPRESSION damp EXPRESSION */
void *sdt_expr_and2(void **sub) { return sdt_expr_and(sub); }

/* EXPRESSION -> EXPRESSION or EXPRESSION */
void *sdt_expr_or(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> EXPRESSION dbar EXPRESSION */
void *sdt_expr_or2(void **sub) { return sdt_expr_or(sub); }

/* EXPRESSION -> ASSIGNMENT */
void *sdt_expr_assign(void **sub) { NOT_IMPLEMENTED; }

/* --- */

/* EXPRESSION -> id */
void *sdt_expr_id(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> bool */
void *sdt_expr_bool(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> int */
void *sdt_expr_int(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> float */
void *sdt_expr_float(void **sub) { NOT_IMPLEMENTED; }

/* EXPRESSION -> string */
void *sdt_expr_string(void **sub) { NOT_IMPLEMENTED; }
