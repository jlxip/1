#include "sdt.h"
#include "../parser/easy/src/Grammar.h"
#include <compiler.h>
#include <stdio.h>

void setup_sdt(void *ptr) {
    Grammar *g = (Grammar *)ptr;

    Grammar_add_output(g, "relpath_dot", sdt_relpath_dot);
    Grammar_add_output(g, "relpath_direct", sdt_relpath_direct);
    Grammar_add_output(g, "module_dot", sdt_module_dot);
    Grammar_add_output(g, "module_direct", sdt_module_direct);
}
