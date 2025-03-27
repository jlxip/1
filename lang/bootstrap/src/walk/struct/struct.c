#include "struct.h"
#include "../annotations/annotations.h"
#include "../type/type.h"
#include <tokens.h>

static map walk_struct_def(AST *ast, const char **names, Symbols *syms);

ObjStruct walk_struct(AST *ast, const char **names, Symbols *syms) {
    ObjStruct ret;
    ObjAnnotations anns;
    Capture *id;
    const char *name;
    map fields;

    anns = walk_annotations(SUB_AST(0), names, syms);
    if (anns) {
        if (map_has(anns, "generic")) {
            ObjAnnotation *generic = map_get(anns, "generic", ObjAnnotation);
            ret.generic = generic->args;
        } else {
            ret.generic = NULL;
        }
    } else {
        ret.generic = NULL;
    }

    id = (Capture *)SUB_AST(2);
    name = (const char *)(id->info);

    ast = SUB_AST(4);
    fields = walk_struct_def(ast, names, syms);

    ret.lineno = id->lineno;
    ret.name = name;
    ret.fields = fields;
    return ret;
}

static map walk_struct_def(AST *ast, const char **names, Symbols *syms) {
    map ret = NULL; /* map<const char*, Type> */
    if (IS_NAME("struct_def_null"))
        throw("empty struct");

    assert(IS_NAME("struct_def"));
    map_new_string(&ret, sizeof(Type), NULL, NULL, NULL, NULL);

    for (;;) {
        AST *next;
        const char *name;
        Type type;

        if (IS_NAME("struct_def_null"))
            break;

        next = SUB_AST(2);
        ast = SUB_AST(0);

        name = (const char *)(((Capture *)SUB_AST(0))->info);
        type = walk_type(SUB_AST(2), names, syms);

        if (map_has(ret, name))
            throwe("multiple fields named %s", name);
        map_add(ret, name, &type);
        ast = next;
    }

    return ret;
}
