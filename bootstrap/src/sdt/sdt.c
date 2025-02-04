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
    ADD(global_struct);
    /* FUNCTION */
    ADD(func_args);
    /* TYPED_IDS */
    ADD(typed_ids_rec);
    ADD(typed_ids_null);

    /* --- EXPRESSIONS (expr.c) --- */
    ADD(expr_par);
    ADD(expr_id);
    ADD(expr_lit);
    ADD(expr_hat);
    ADD(expr_amp);
    ADD(expr_bar);
    ADD(expr_star);
    ADD(expr_slash);
    ADD(expr_plus);
    ADD(expr_minus);
    ADD(expr_is);
    ADD(expr_deq);
    ADD(expr_neq);
    ADD(expr_lt);
    ADD(expr_leq);
    ADD(expr_gt);
    ADD(expr_geq);
    ADD(expr_not);
    ADD(expr_and);
    ADD(expr_and2);
    ADD(expr_or);
    ADD(expr_or2);
    ADD(expr_assign);

    /* --- LITERALS (lit.c) --- */
    ADD(lit_bool);
    ADD(lit_int);
    ADD(lit_float);
    ADD(lit_string);

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

    /* --- CONTROL (ctrl.c) --- */
    ADD(ctrl_assert);
    ADD(ctrl_break);
    ADD(ctrl_continue);
    ADD(ctrl_fall);
    ADD(ctrl_ret);
    ADD(ctrl_retval);

    /* --- CONDITIONAL (cond.c) --- */
    ADD(cond_if);
    ADD(cond_switch);
    ADD(if_only);
    ADD(if_elif);
    ADD(elif_only);
    ADD(elif_rec);
    ADD(elif_else);
    ADD(else);
    ADD(switch);
    ADD(switch_rec);
    ADD(switch_one);
    ADD(switch_case);
    ADD(switch_default);

    /* --- LOOP (loop.c) --- */
    ADD(loop_for);
    ADD(loop_while);
    ADD(for);
    ADD(while);

    /* --- STRUCT (struct.c) --- */
    ADD(struct);
    ADD(struct_def);
    ADD(struct_def_null);

    /* --- BLOCK (block.c) --- */
    ADD(block);
    ADD(stmts_rec);
    ADD(stmts_null);
    ADD(stmt_block);
    ADD(stmt_expr);
    ADD(stmt_ctrl);
    ADD(stmt_cond);
    ADD(stmt_loop);

    /* Check all NTs have an output function */
    do {
        size_t i;
        for (i = 1; i < buffer_num(g->outputs); ++i)
            if (!*buffer_get(g->outputs, i, void *))
                throwe("production without an output: \"%s\"",
                    *buffer_get(g->prod2name, i, char *));
    } while (0);
}
