#include "sdt.h"
#include "../parser/easy/src/Grammar.h"
#include <compiler.h>
#include <stdio.h>

#define ADD(X)                                                                 \
    do {                                                                       \
        void *sdt_##X(void **sub);                                             \
        Grammar_add_output(g, #X, sdt_##X);                                    \
    } while (0)

void setup_sdt(void *ptr) {
    Grammar *g = (Grammar *)ptr;

    /* --- GLOBAL SCOPE (global.c) --- */
    /* S */
    ADD(program);
    /* USES */
    ADD(uses_rec);
    ADD(uses_direct);
    /* USE */
    ADD(use);
    /* RELATIVE_PATH */
    ADD(relpath_dot);
    ADD(relpath_direct);
    /* MODULE */
    ADD(module_dot);
    ADD(module_direct);

    /* GLOBALS */
    ADD(globals_rec);
    ADD(globals_direct);
    /* GLOBAL */
    ADD(global_assign);
    ADD(global_func);
    /* FUNCTION */
    ADD(func_args);
    ADD(func_direct);
    /* TYPED_IDS */
    ADD(typed_ids_rec);
    ADD(typed_ids_direct);

    /* --- EXPRESSIONS (expr.c) --- */
    ADD(expr_par);
    ADD(expr_hat);
    ADD(expr_amp);
    ADD(expr_bar);
    ADD(expr_star);
    ADD(expr_slash);
    ADD(expr_plus);
    ADD(expr_minus);
    ADD(expr_deq);
    ADD(expr_neq);
    ADD(expr_lt);
    ADD(expr_leq);
    ADD(expr_gt);
    ADD(expr_geq);
    ADD(expr_not);
    ADD(expr_and);
    ADD(expr_or);
    ADD(expr_assign);
    ADD(expr_id);
    ADD(expr_bool);
    ADD(expr_int);
    ADD(expr_float);
    ADD(expr_string);

    /* --- ASSIGNMENT (assign.c) --- */
    ADD(assign_eq);
    ADD(assign_plus);
    ADD(assign_minus);
    ADD(assign_star);
    ADD(assign_slash);
    ADD(assign_hat);
    ADD(assign_amp);
    ADD(assign_bar);
    /* LHS */
    ADD(lhs_typed_rec);
    ADD(lhs_typed);
    ADD(lhs_id_rec);
    ADD(lhs_id);
    /* TYPED_ID and TYPES in general */
    ADD(typed);
    ADD(type_typed);
    ADD(type_direct);
    ADD(types_rec);
    ADD(types_direct);
    /* RHS */
    ADD(rhs_rec);
    ADD(rhs_direct);

    /* --- BLOCK (block.c) --- */
    ADD(block);
    ADD(statements_rec);
    ADD(statements_direct);
    ADD(statement_expr);

    /* Check all NTs have an output function */
    do {
        size_t i;
        for (i = 1; i < buffer_num(g->outputs); ++i)
            if (!*buffer_get(g->outputs, i, void *))
                throwe("production without an output: \"%s\"",
                    *buffer_get(g->prod2name, i, char *));
    } while (0);
}
