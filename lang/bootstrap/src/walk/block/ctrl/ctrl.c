#include "ctrl.h"
#include "../../expr/expr.h"

void walk_ctrl(AST *ast, const char **names, Symbols *syms) {
    (void)syms;

    if (IS_NAME("ctrl_assert")) {
        todo();
    } else if (IS_NAME("ctrl_break")) {
        todo();
    } else if (IS_NAME("ctrl_continue")) {
        todo();
    } else if (IS_NAME("ctrl_fall")) {
        todo();
    } else if (IS_NAME("ctrl_ret")) {
        todo();
    } else if (IS_NAME("ctrl_retval")) {
        (void)walk_expr(SUB_AST(1), names, syms);
    } else
        UNREACHABLE;
}
