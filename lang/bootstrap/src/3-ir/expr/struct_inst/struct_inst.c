#include "struct_inst.h"
#include "../../lookup/lookup.h"
#include "../../struct/struct.h"
#include "../expr.h"

static map walk_sfinst(WalkCtx *ctx, AST *ast);

ObjStructInst walk_struct_inst(WalkCtx *ctx, AST *ast) {
    ObjStructInst ret;
    Declaration *struct_def;
    ObjStruct *sd;
    map fills; /* map<char*, ObjExpression> */
    map_iterator it;

    assert(IS_NAME("struct_inst"));
    ret.mst = (TokenIdx)SUB_AST(1); /* { */
    struct_def = lookup(ctx, SUB_AST(0));
    ret.type = struct_def->type;
    if (struct_def->type.id == TYPE_STRUCT_DEF) {
        sd = (ObjStruct *)(struct_def->type.data);
        if (sd->generic)
            throw("cannot instantiate a generic struct without specifics");
    } else if (struct_def->type.id == TYPE_STRUCT_SPECIFIC) {
        ObjSpecificStruct *ss;
        ss = (ObjSpecificStruct *)(struct_def->type.data);
        sd = (ObjStruct *)(ss->struct_def->type.data);
    } else {
        throw("can only instantiate a struct");
    }

    fills = walk_sfinst(ctx, SUB_AST(2));
    if (map_num(fills) != map_num(sd->fields))
        throw("invalid number of fields in struct instantiation");

    it = map_it_begin(fills);
    while (!map_it_finished(&it)) {
        const char *name = map_it_get_key(&it, char);
        Type field_type, fill_type;

        /* Check it exists */
        if (!map_has(sd->fields, name))
            throwe("invalid field in struct instantiation: %s", name);

        /* Check type */
        field_type = *map_get(sd->fields, name, Type);
        fill_type = map_it_get_value(&it, ObjExpression)->type;
        if (field_type.id != fill_type.id)
            throw("struct field type mismatch");
        /* This compares pointers which is not ideal and will surely fail */
        if (field_type.data != fill_type.data)
            throw("struct field type mismatch");

        map_it_next(&it);
    }

    ret.fills = fills;
    return ret;
}

static map walk_sfinst(WalkCtx *ctx, AST *ast) {
    map ret = NULL; /* map<char*, ObjExpression> */
    map_new_string(&ret, sizeof(ObjExpression), NULL, NULL, NULL, NULL);

    if (IS_NAME("struct_field_inst_null"))
        throw("empty struct instantiation");
    assert(IS_NAME("struct_field_inst_rec"));

    for (;;) {
        const char *name;
        ObjExpression expr;

        if (IS_NAME("struct_field_inst_null"))
            break;
        assert(IS_NAME("struct_field_inst_rec"));

        name = TOKEN((TokenIdx)SUB_AST(0))->data.str;
        expr = walk_expr(ctx, SUB_AST(2));

        map_add(ret, name, &expr);
        ast = SUB_AST(4);
    }

    return ret;
}
