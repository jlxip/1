#include "struct_inst.h"
#include "../../lookup/lookup.h"
#include "../../struct/struct.h"

void walk_struct_inst(AST *ast, const char **names, Symbols *syms) {
    Declaration *struct_def;

    assert(IS_NAME("struct_inst"));
    struct_def = lookup(SUB_AST(0), names, syms);
    if (struct_def->type.id == TYPE_STRUCT_DEF) {
        ObjStruct *sd = (ObjStruct *)(struct_def->type.data);
        if (sd->generic)
            throw("cannot instantiate a generic struct without specifics");
    } else if (struct_def->type.id == TYPE_STRUCT_SPECIFIC) {
        /* nothing to do? */
    } else {
        throw("can only instantiate a struct");
    }
}
