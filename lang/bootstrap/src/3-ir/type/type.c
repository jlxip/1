#include "type.h"

static ObjTupleDef *walk_tupledef(WalkCtx *ctx, AST *ast);

Type walk_type(WalkCtx *ctx, AST *ast) {
    Type ret;

    if (IS_NAME("type_bool")) {
        ret.id = TYPE_BOOL;
        ret.data = NULL;
    } else if (IS_NAME("type_byte")) {
        ret.id = TYPE_BYTE;
        ret.data = NULL;
    } else if (IS_NAME("type_float")) {
        ret.id = TYPE_FLOAT;
        ret.data = NULL;
    } else if (IS_NAME("type_ptr")) {
        ret.id = TYPE_PTR;
        ret.data = NULL;
    } else if (IS_NAME("type_string")) {
        ret.id = TYPE_STRING;
        ret.data = NULL;
    } else if (IS_NAME("type_word")) {
        ret.id = TYPE_WORD;
        ret.data = NULL;
    } else if (IS_NAME("type_direct")) {
        /*Token *id = (Token *)(SUB_AST(0));*/
        todo();
    } else if (IS_NAME("type_tuple")) {
        ret.id = TYPE_TUPLE;
        ret.data = walk_tupledef(ctx, SUB_AST(0));
    } else if (IS_NAME("type_tuple_star")) {
        ObjTupleDef *td = walk_tupledef(ctx, SUB_AST(0));
        todo();
        /* td->rep run constant-time expression */
        ret.id = TYPE_TUPLE;
        ret.data = td;
    } else
        UNREACHABLE;

    return ret;
}

static ObjTupleDef *walk_tupledef(WalkCtx *ctx, AST *ast) {
    ObjTupleDef *ret;
    Type aux;
    assert(IS_NAME("tupledef_one") || IS_NAME("tupledef_many"));

    ret = malloc(sizeof(ObjTupleDef));
    ret->mst = (iToken)SUB_AST(0);
    ret->fields = NULL;
    buffer_new(&ret->fields, sizeof(Type));
    ret->rep = 1;

    aux = walk_type(ctx, SUB_AST(1));
    buffer_push(ret->fields, &aux);
    if (IS_NAME("tupledef_many")) {
        ast = SUB_AST(3);
        assert(IS_NAME("types_rec") || IS_NAME("types_direct"));
        for (;;) {
            aux = walk_type(ctx, SUB_AST(0));
            buffer_push(ret->fields, &aux);
            if (IS_NAME("types_direct"))
                break;
            ast = SUB_AST(3);
        }
    }

    return ret;
}
