#include "emit.h"
#include <files.h>
#include <string.h>

#define EMIT(X, N) emit_##X(ctx, GET_IRID(N))
#define EMITT(X, N) emit_##X(ctx, GET_IRID(N), GET_IRTYPE(N))
#define TYPE(N) buffer_get(ctx->sem.types, N, Type)
#define TYPE_CHILD(N) TYPE(GET_IRID(N))

#define IIR_BEGIN (buffer_num(ctx.irs) - 3)
#define IIR_CONST_SELF (buffer_num(ctx->irs) - 2)
#define IIR_MUT_SELF (buffer_num(ctx->irs) - 1)

static const size_t ONE = 1;

static const char *get_id(Ctx *ctx, iToken itoken) {
    Token *token = GET_TOKEN(itoken);
    assert(token->id == T_ID);
    return token->data.str;
}

static size_t get_bool(Ctx *ctx, iToken itoken) {
    Token *token = GET_TOKEN(itoken);
    assert(token->id == T_BOOL);
    return token->data.word;
}

static const char *get_string(Ctx *ctx, iToken itoken) {
    Token *token = GET_TOKEN(itoken);
    assert(token->id == T_STRING);
    return token->data.str;
}

static const char *get_word(Ctx *ctx, iToken itoken) {
    Token *token = GET_TOKEN(itoken);
    assert(token->id == T_WORD);
    return token->data.str;
}

static const char *get_float(Ctx *ctx, iToken itoken) {
    Token *token = GET_TOKEN(itoken);
    assert(token->id == T_FLOAT);
    return token->data.str;
}

static void push_decls(Ctx *ctx) {
    buffer empty = NULL;
    buffer_new(&empty, sizeof(string));
    buffer_push(ctx->decl, &empty);
}

static buffer pop_decls(Ctx *ctx) {
    buffer ret = *buffer_back(ctx->decl, buffer);
    buffer_pop(ctx->decl);
    return ret;
}

static void countit(Ctx *ctx, string *prefix) {
    size_t n;
    if (map_has(ctx->decln, sget(*prefix))) {
        n = (*map_get(ctx->decln, sget(*prefix), size_t))++;
    } else {
        n = 0;
        map_add(ctx->decln, sget(*prefix), &ONE);
    }
    saddi(prefix, n);
}

static void push_decl(Ctx *ctx, string type, string name, string value) {
    string line;

    /* type */
    line = sdup(type);
    saddc(&line, " ");
    /* name */
    sadd(&line, name);
    /* value */
    if (value.len) {
        saddc(&line, " = ");
        sadd(&line, value);
    }
    saddc(&line, ";");

    assert(buffer_num(ctx->decl));
    buffer_push(*buffer_back(ctx->decl, buffer), &line);
}

/* --- */

static string emit_type(Ctx *ctx, Type type) {
    switch (type.id) {
    case TYPE_UNKNOWN:
        throw("tried to emit TYPE_UNKNOWN! (bug)");
        break;
    case TYPE_NOTHING:
        return sc("void");
    case TYPE_BOOL:
        return sc("unsigned char");
    case TYPE_BYTE:
        return sc("unsigned char");
    case TYPE_FLOAT:
        return sc("double");
    case TYPE_PTR:
        return sc("void*");
    case TYPE_STRING:
        return sc("string");
    case TYPE_WORD:
        return sc("size_t");
    case TYPE_TUPLE:
        todo();
        break;
    case TYPE_FUNC:
        UNREACHABLE;
        break;
    case TYPE_STRUCT_DEF:
        throw("this is impossible");
        break;
    case TYPE_STRUCT_INST:
        return sc(*buffer_get(ctx->sem.mangling, type.data.word, const char *));
    case TYPE_MODULE:
        UNREACHABLE;
        break;
    default:
        UNREACHABLE;
    }

    return sc("unreachable");
}

static string emit_lit(Ctx *ctx, iIR iir, IRType type) {
    string ret = snew();
    IR *ir = GET_IR(iir);

    switch (type) {
    case IR_lit_bool:
        saddi(&ret, get_bool(ctx, GET_IRID(0)));
        break;
    case IR_lit_word:
        saddc(&ret, get_word(ctx, GET_IRID(0)));
        break;
    case IR_lit_float:
        saddc(&ret, get_float(ctx, GET_IRID(0)));
        break;
    case IR_lit_string: {
        const char *str = get_string(ctx, GET_IRID(0));
        string val;

        ret = sc("_Xlits");
        countit(ctx, &ret);

        val = sc("{");
        saddi(&val, strlen(str));
        saddc(&val, ", \"");
        saddc(&val, str);
        saddc(&val, "\"}");

        push_decl(ctx, sc("const string"), ret, val);
        break;
    }
    case IR_lit_tuple:
        todo();
        break;
    default:
        UNREACHABLE;
    }

    return ret;
}

static Expr emit_expr(Ctx *ctx, iIR iir, IRType type);
static Expr emit_expr_list(Ctx *ctx, iIR iir, IRType type) {
    Expr ret, sub;
    IR *ir = GET_IR(iir);

    ret.code = snew();
    ret.above = NULL;
    buffer_new(&ret.above, sizeof(string));
    ret.lvalue = 0;

    for (;;) {
        switch (type) {
        case IR_expression_list_one:
            sub = EMITT(expr, 0);
            sadd(&ret.code, sub.code);
            buffer_join(ret.above, sub.above);
            return ret;
        case IR_expression_list_rec:
            sub = EMITT(expr, 0);
            sadd(&ret.code, sub.code);
            saddc(&ret.code, ", ");
            buffer_join(ret.above, sub.above);

            iir = GET_IRID(2);
            type = GET_IRTYPE(2);
            ir = GET_IR(iir);
            break;
        default:
            UNREACHABLE;
        }
    }
}

static Expr emit_struct_inst(Ctx *ctx, iIR iir) {
    Expr ret;
    IR *ir = GET_IR(iir);
    iIR base;
    string struct_name, name;
    IRType irtype;

    ret.above = NULL;
    buffer_new(&ret.above, sizeof(string));
    ret.self_val = snew(); /* Same as ret.code */
    ret.lvalue = 1;

    /* Declaration */
    base = TYPE(iir)->data.word;
    struct_name = sc(*buffer_get(ctx->sem.mangling, base, const char *));
    name = sc("_Xinst");
    countit(ctx, &name);
    push_decl(ctx, struct_name, name, snew());

    /* Fill fields */
    iir = GET_IRID(2);
    irtype = GET_IRTYPE(2);
    for (;;) {
        ir = GET_IR(iir);

        switch (irtype) {
        case IR_struct_field_inst_rec: {
            string assign;
            string field;
            Expr sub;

            assert(GET_IRTYPE(0) == IR_TOKEN);
            field = sc(get_id(ctx, GET_IRID(0)));
            sub = EMITT(expr, 2);
            buffer_join(ret.above, sub.above);

            assign = sdup(name);
            saddc(&assign, ".");
            sadd(&assign, field);
            saddc(&assign, " = ");
            sadd(&assign, sub.code);
            saddc(&assign, ";");
            buffer_push(ret.above, &assign);

            iir = GET_IRID(4);
            irtype = GET_IRTYPE(4);
            break;
        }
        case IR_struct_field_inst_null:
            goto next;
        default:
            UNREACHABLE;
        }
    }

next:
    ret.code = name;
    return ret;
}

static Expr emit_assign(Ctx *ctx, iIR iir, IRType type) {
    Expr ret, sub;
    IR *ir = GET_IR(iir);

    ret.code = snew();
    ret.above = NULL;
    buffer_new(&ret.above, sizeof(string));
    ret.lvalue = 0; /* I'm not sure */

    switch (type) {
    case IR_assign_eq:
        /* TODO: if reference, dereference */
        sub = EMITT(expr, 0);
        sadd(&ret.code, sub.code);
        buffer_join(ret.above, sub.above);
        saddc(&ret.code, " = ");
        sub = EMITT(expr, 2);
        sadd(&ret.code, sub.code);
        buffer_join(ret.above, sub.above);
        break;
    case IR_assign_plus:
    case IR_assign_minus:
    case IR_assign_star:
    case IR_assign_slash:
    case IR_assign_hat:
    case IR_assign_amp:
    case IR_assign_bar:
        todo();
        break;
    default:
        UNREACHABLE;
    }

    return ret;
}

static Expr emit_expr(Ctx *ctx, iIR iir, IRType type) {
    Expr ret, sub, sub2;
    IR *ir = GET_IR(iir);

    ret.code = snew();
    ret.above = NULL;
    buffer_new(&ret.above, sizeof(string));
    ret.self_val = snew();
    ret.lvalue = 0; /* sane default */

    switch (type) {
    case IR_expr_par:
        sub = EMITT(expr, 1);
        saddc(&ret.code, "(");
        sadd(&ret.code, sub.code);
        saddc(&ret.code, ")");
        buffer_join(ret.above, sub.above);
        ret.lvalue = sub.lvalue;
        break;
    case IR_expr_id:
        ret.code = sc(*buffer_get(ctx->sem.mangling, iir, const char *));
        ret.self_val = ret.code;
        ret.lvalue = 1;
        break;
    case IR_expr_dot: {
        string name = sc(get_id(ctx, GET_IRID(2)));
        sub = EMITT(expr, 0);
        buffer_join(ret.above, sub.above);

        switch (TYPE_CHILD(0)->id) {
        case TYPE_STRUCT_DEF: {
            Struct *obj = (Struct *)(TYPE_CHILD(0)->data.ptr);
            if (map_has(obj->fields, sget(name))) {
                todo();
                ret.lvalue = 1;
            } else if (map_has(obj->methods, sget(name))) {
                iIR method = *map_get(obj->methods, sget(name), iIR);
                saddc(&ret.code,
                    *buffer_get(ctx->sem.mangling, method, const char *));
                ret.lvalue = 1;
            } else
                UNREACHABLE;
            break;
        }
        case TYPE_STRUCT_INST: {
            iIR base;
            Struct *obj;
            base = TYPE_CHILD(0)->data.word;
            obj = (Struct *)(TYPE(base)->data.ptr);

            if (map_has(obj->fields, sget(name))) {
                assert(sub.self_val.len);
                sadd(&ret.code, sub.self_val);
                saddc(&ret.code, "."); /* TODO: depends if ref */
                sadd(&ret.code, name);
                ret.lvalue = 1;
            } else if (map_has(obj->methods, sget(name))) {
                iIR method = *map_get(obj->methods, sget(name), iIR);
                saddc(&ret.code,
                    *buffer_get(ctx->sem.mangling, method, const char *));
                if (sub.lvalue) {
                    assert(sub.self_val.len);
                    ret.self_val = sc("&"); /* TODO: depends if ref */
                    sadd(&ret.self_val, sub.self_val);
                } else {
                    /* Make auxiliary and use that */
                    string struct_name, name, above;

                    struct_name =
                        sc(*buffer_get(ctx->sem.mangling, base, const char *));
                    name = sc("_Xinst");
                    countit(ctx, &name);
                    push_decl(ctx, struct_name, name, snew());

                    above = sdup(name);
                    saddc(&above, " = ");
                    sadd(&above, sub.code);
                    saddc(&above, ";");
                    buffer_push(ret.above, &above);

                    ret.self_val = sc("&");
                    sadd(&ret.self_val, name);
                }
                ret.lvalue = 1;
            } else
                UNREACHABLE;
            break;
        }
        case TYPE_SELF: {
            Struct *obj;
            assert(ctx->self_struct);
            obj = (Struct *)(TYPE(ctx->self_struct)->data.ptr);

            if (map_has(obj->fields, sget(name))) {
                saddc(&ret.code, "self->");
                sadd(&ret.code, name);
                ret.lvalue = 1;
            } else if (map_has(obj->methods, sget(name))) {
                iIR method = *map_get(obj->methods, sget(name), iIR);
                saddc(&ret.code,
                    *buffer_get(ctx->sem.mangling, method, const char *));
                ret.self_val = sc("self");
                ret.lvalue = 1;
            } else
                UNREACHABLE;
            break;
        }
        default:
            UNREACHABLE;
        }
        break;
    }
    case IR_expr_call_noargs:
        sub = EMITT(expr, 0);
        sadd(&ret.code, sub.code);
        buffer_join(ret.above, sub.above);
        if (IS_METHOD(GET_IRID(0)) && !IS_STATIC(GET_IRID(0))) {
            saddc(&ret.code, "(");
            assert(sub.self_val.len);
            sadd(&ret.code, sub.self_val);
            saddc(&ret.code, ")");
        } else {
            saddc(&ret.code, "()");
        }
        break;
    case IR_expr_call:
        sub = EMITT(expr, 0);
        sadd(&ret.code, sub.code);
        buffer_join(ret.above, sub.above);
        saddc(&ret.code, "(");
        if (IS_METHOD(GET_IRID(0)) && !IS_STATIC(GET_IRID(0))) {
            assert(sub.self_val.len);
            sadd(&ret.code, sub.self_val);
            saddc(&ret.code, ", ");
        }
        sub = EMITT(expr_list, 2);
        sadd(&ret.code, sub.code);
        buffer_join(ret.above, sub.above);
        saddc(&ret.code, ")");
        break;
    case IR_expr_lit:
        sadd(&ret.code, EMITT(lit, 0));
        break;
    case IR_expr_sizeof:
    case IR_expr_hat:
    case IR_expr_amp:
    case IR_expr_bar:
        todo();
        break;
    case IR_expr_star:
        sub = EMITT(expr, 0);
        buffer_join(ret.above, sub.above);
        sub2 = EMITT(expr, 2);
        buffer_join(ret.above, sub2.above);

        switch (TYPE_CHILD(0)->id) {
        case TYPE_WORD:
            sadd(&ret.code, sub.code);
            saddc(&ret.code, " * ");
            sadd(&ret.code, sub2.code);
            break;
        default:
            todo();
        }
        break;
    case IR_expr_slash:
    case IR_expr_plus:
    case IR_expr_minus:
    case IR_expr_deq:
    case IR_expr_neq:
    case IR_expr_lt:
    case IR_expr_leq:
    case IR_expr_gt:
    case IR_expr_geq:
    case IR_expr_not:
    case IR_expr_and:
    case IR_expr_or:
        todo();
        break;
    case IR_expr_struct_inst:
        sub = EMIT(struct_inst, 0);
        sadd(&ret.code, sub.code);
        buffer_join(ret.above, sub.above);
        ret.self_val = sub.code;
        break;
    case IR_expr_assign:
        sub = EMITT(assign, 0);
        sadd(&ret.code, sub.code);
        buffer_join(ret.above, sub.above);
        break;
    default:
        UNREACHABLE;
    }

    return ret;
}

static string emit_decl(Ctx *ctx, iIR iir, IRType type) {
    string typename, name;
    Expr expr;
    IR *ir = GET_IR(iir);
    const char *mangled;
    size_t i;
    string ret = snew();

    mangled = *buffer_get(ctx->sem.mangling, iir, const char *);
    assert(mangled);

    switch (type) {
    case IR_decl_id:
        typename = emit_type(ctx, *TYPE(iir));
        name = sc(mangled);
        expr = EMITT(expr, 3);
        break;
    case IR_decl_p_id:
        typename = emit_type(ctx, *TYPE(iir));
        name = sc(mangled);
        expr = EMITT(expr, 4);
        break;
    default:
        UNREACHABLE;
    }

    for (i = 0; i < buffer_num(expr.above); ++i)
        saddln(&ret, *buffer_get(expr.above, i, string));

    if (buffer_empty(ctx->decl)) {
        /* Global, declaration goes right here right now */
        /* These can be actually const ;) */
        if (!IS_MUTABLE(iir))
            saddc(&ret, "const ");
        sadd(&ret, typename);
        saddc(&ret, " ");
        sadd(&ret, name);
        saddc(&ret, " = ");
        sadd(&ret, expr.code);
        saddc(&ret, ";");
    } else {
        /* Scoped, declaration is saved to be put at the top of the block */
        push_decl(ctx, typename, name, snew());
        /* Assignment right here right now */
        sadd(&ret, name);
        saddc(&ret, " = ");
        sadd(&ret, expr.code);
        saddc(&ret, ";");
    }

    return ret;
}

static string emit_block(Ctx *ctx, iIR iir);
static string emit_stmt(Ctx *ctx, iIR iir, IRType type) {
    string ret = snew();
    IR *ir = GET_IR(iir);

    switch (type) {
    case IR_stmt_block:
        saddln(&ret, EMIT(block, 0));
        break;
    case IR_stmt_decl:
        saddln(&ret, EMITT(decl, 0));
        break;
    case IR_stmt_expr: {
        Expr expr = EMITT(expr, 0);
        size_t i;
        for (i = 0; i < buffer_num(expr.above); ++i)
            saddln(&ret, *buffer_get(expr.above, i, string));
        sadd(&ret, expr.code);
        saddlnc(&ret, ";");
        break;
    }
    case IR_stmt_assert: {
        Expr expr = EMITT(expr, 1);
        size_t i;

        switch (TYPE_CHILD(1)->id) {
        case TYPE_BOOL:
        case TYPE_BYTE:
        case TYPE_FLOAT:
        case TYPE_PTR:
        case TYPE_STRING:
        case TYPE_WORD:
            for (i = 0; i < buffer_num(expr.above); ++i)
                saddln(&ret, *buffer_get(expr.above, i, string));
            saddc(&ret, "_Xassert(");
            sadd(&ret, expr.code);
            saddlnc(&ret, ");");
            break;
        case TYPE_STRUCT_INST:
            todo();
            break;
        default:
            UNREACHABLE;
        }
        break;
    }
    case IR_stmt_break:
        todo();
        break;
    case IR_stmt_continue:
        todo();
        break;
    case IR_stmt_fall:
        todo();
        break;
    case IR_stmt_ret:
        saddlnc(&ret, "return;");
        break;
    case IR_stmt_retval: {
        Expr expr = EMITT(expr, 1);
        size_t i;
        for (i = 0; i < buffer_num(expr.above); ++i)
            saddln(&ret, *buffer_get(expr.above, i, string));
        saddc(&ret, "return ");
        sadd(&ret, expr.code);
        saddlnc(&ret, ";");
        break;
    }
    case IR_stmt_if:
        todo();
        break;
    case IR_stmt_switch:
        todo();
        break;
    case IR_stmt_for:
        todo();
        break;
    case IR_stmt_while:
        todo();
        break;
    default:
        UNREACHABLE;
    }

    return ret;
}

static string emit_stmts(Ctx *ctx, iIR iir, IRType type) {
    string ret = snew();

    for (;;) {
        IR *ir = GET_IR(iir);

        switch (type) {
        case IR_stmts_rec:
            sadd(&ret, EMITT(stmt, 0));
            iir = GET_IRID(1);
            type = GET_IRTYPE(1);
            break;
        case IR_stmts_null:
            return ret;
        default:
            UNREACHABLE;
        }
    }
}

static string emit_block(Ctx *ctx, iIR iir) {
    string ret = sc("{");
    IR *ir = GET_IR(iir);
    string body;
    buffer decls;
    size_t i;

    snewln(&ret);

    push_decls(ctx);
    body = EMITT(stmts, 1);
    decls = pop_decls(ctx);
    for (i = 0; i < buffer_num(decls); ++i)
        saddln(&ret, *buffer_get(decls, i, string));
    if (i)
        snewln(&ret);
    sadd(&ret, body);
    saddlnc(&ret, "}");

    return ret;
}

static string emit_param(Ctx *ctx, iIR iir, IRType type) {
    string ret = snew();

    switch (type) {
    case IR_param_copy:
        saddc(&ret, "const ");
        sadd(&ret, emit_type(ctx, *TYPE(iir)));
        break;
    case IR_param_ref:
        todo();
        break;
    case IR_param_mut:
        todo();
        break;
    case IR_param_mutref:
        todo();
        break;
    default:
        UNREACHABLE;
    }

    saddc(&ret, " ");
    saddc(&ret, *buffer_get(ctx->sem.mangling, iir, const char *));
    return ret;
}

static string emit_params(Ctx *ctx, iIR iir, IRType type) {
    string ret = snew();

    for (;;) {
        IR *ir = GET_IR(iir);

        switch (type) {
        case IR_params_rec:
            sadd(&ret, EMITT(param, 0));
            saddc(&ret, ", ");
            iir = GET_IRID(1);
            type = GET_IRTYPE(1);
            break;
        case IR_params_one:
            sadd(&ret, EMITT(param, 0));
            return ret;
        default:
            UNREACHABLE;
        }
    }

    return ret;
}

static string emit_func(Ctx *ctx, iIR iir, IRType type) {
    string ret = sc("static ");
    IR *ir = GET_IR(iir);
    const char *mangled;
    Function *func;
    size_t block;
    bool colon_mode = false;

    mangled = *buffer_get(ctx->sem.mangling, iir, const char *);
    assert(mangled);
    func = (Function *)(TYPE(iir)->data.ptr);

    if (func->ret.id != TYPE_NOTHING) {
        sadd(&ret, emit_type(ctx, func->ret));
        saddc(&ret, " ");
    } else {
        saddc(&ret, "void ");
    }

    saddc(&ret, mangled);
    if (func->params) {
        /* We have params */
        saddc(&ret, "(");
        if (IS_METHOD(iir) && !IS_STATIC(iir)) {
            saddc(&ret, "const ");
            saddc(&ret,
                *buffer_get(ctx->sem.mangling, ctx->self_struct, const char *));
            saddc(&ret, " *self, ");
        }
        sadd(&ret, EMITT(params, 4));
        saddc(&ret, ") ");
    } else {
        /* No params */
        if (IS_METHOD(iir) && !IS_STATIC(iir)) {
            saddc(&ret, "(const ");
            saddc(&ret,
                *buffer_get(ctx->sem.mangling, ctx->self_struct, const char *));
            saddc(&ret, " *self)");
        } else {
            saddc(&ret, "(void) ");
        }
    }

    switch (type) {
    case IR_function_noargs_void:
        block = 3;
        break;
    case IR_function_noargs_void_colon:
        block = 4;
        colon_mode = true;
        break;
    case IR_function_noargs_typed:
        block = 5;
        break;
    case IR_function_noargs_typed_colon:
        block = 6;
        colon_mode = true;
        break;
    case IR_function_void:
        block = 6;
        break;
    case IR_function_void_colon:
        block = 7;
        colon_mode = true;
        break;
    case IR_function_typed:
        block = 8;
        break;
    case IR_function_typed_colon:
        block = 9;
        colon_mode = true;
        break;
    default:
        UNREACHABLE;
    }

    if (colon_mode) {
        Expr expr;
        buffer decls;
        size_t i;

        push_decls(ctx);
        expr = EMITT(expr, block);
        decls = pop_decls(ctx);

        saddlnc(&ret, "{");
        /* Declarations */
        for (i = 0; i < buffer_num(decls); ++i)
            saddln(&ret, *buffer_get(decls, i, string));
        if (i)
            snewln(&ret);
        /* Auxiliary assigns */
        for (i = 0; i < buffer_num(expr.above); ++i)
            saddln(&ret, *buffer_get(expr.above, i, string));
        /* Return */
        saddc(&ret, "return ");
        saddln(&ret, expr.code);
        saddlnc(&ret, ";");
        saddc(&ret, "}");
    } else {
        sadd(&ret, EMIT(block, block));
    }

    return ret;
}

static string emit_struct(Ctx *ctx, iIR iir) {
    string ret = sc("typedef struct {");
    Struct *obj;
    map_iterator it;
    const char *mangled;

    obj = (Struct *)(TYPE(iir)->data.ptr);
    it = map_it_begin(obj->fields);
    while (!map_it_finished(&it)) {
        const char *field_name;
        Type field_type;

        field_name = map_it_get_key(&it, const char);
        field_type = *map_it_get_value(&it, Type);

        snewln(&ret);
        sadd(&ret, emit_type(ctx, field_type));
        saddc(&ret, " ");
        saddc(&ret, field_name);
        saddc(&ret, ";");

        map_it_next(&it);
    }

    snewln(&ret);
    saddc(&ret, "} ");

    mangled = *buffer_get(ctx->sem.mangling, iir, const char *);
    assert(mangled);
    saddc(&ret, mangled);
    saddc(&ret, ";");

    return ret;
}

static string emit_impl(Ctx *ctx, iIR iir) {
    string ret = snew();
    IR *ir = GET_IR(iir);
    IRType type;

    ctx->self_struct = TYPE(iir)->data.word;

    iir = GET_IRID(4);
    type = GET_IRTYPE(4);
    for (;;) {
        ir = GET_IR(iir);
        switch (type) {
        case IR_impl_def:
            saddln(&ret, EMITT(func, 0));
            iir = GET_IRID(1);
            type = GET_IRTYPE(1);
            break;
        case IR_impl_def_null:
            goto next;
        default:
            UNREACHABLE;
        }
    }

next:
    ctx->self_struct = 0;
    return ret;
}

static string emit_extern(Ctx *ctx, iIR iir) {
    string ret;
    IR *ir = GET_IR(iir);
    const char *name;
    Function *func;

    name = *buffer_get(ctx->sem.mangling, iir, const char *);
    assert(name);
    func = (Function *)(TYPE(iir)->data.ptr);

    if (func->ret.id != TYPE_NOTHING) {
        ret = emit_type(ctx, func->ret);
        saddc(&ret, " ");
    } else {
        ret = sc("void ");
    }

    saddc(&ret, name);
    if (func->params) {
        saddc(&ret, "(");
        sadd(&ret, EMITT(params, 4));
        saddc(&ret, ");");
    } else {
        saddc(&ret, "(void);");
    }

    return ret;
}

static string emit_global(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);

    switch (type) {
    case IR_global_decl:
        return EMITT(decl, 0);
    case IR_global_func:
        return EMITT(func, 0);
    case IR_global_struct:
        return EMIT(struct, 0);
    case IR_global_impl:
        return EMIT(impl, 0);
    case IR_global_extern:
        return EMIT(extern, 0);
    default:
        UNREACHABLE;
    }

    return snew();
}

static string emit_globals(Ctx *ctx, iIR iir, IRType type) {
    string ret = snew();

    for (;;) {
        IR *ir = GET_IR(iir);

        switch (type) {
        case IR_globals_rec:
            saddln(&ret, EMITT(global, 0));
            snewln(&ret);
            iir = GET_IRID(1);
            type = GET_IRTYPE(1);
            break;
        case IR_globals_null:
            return ret;
        default:
            UNREACHABLE;
        }
    }

    return ret;
}

static string emit_program(Ctx *ctx, iIR iir) {
    string ret = snew();
    IR *ir = GET_IR(iir);

    /* Uses */
    /* TODO */

    /* Globals */
    sadd(&ret, EMITT(globals, 1));

    return ret;
}

const char PREAMBLE_PATH[] = "src/4-emit/preamble.txt";
const char EPILOGUE_PATH[] = "src/4-emit/epilogue.txt";

char *emit(Tokens tokens, IRs irs, SemResult sem) {
    Ctx ctx;
    string ret;

    ctx.tokens = tokens;
    ctx.irs = irs;
    ctx.sem = sem;
    ctx.decl = NULL;
    ctx.decln = NULL;
    ctx.self_struct = 0;

    buffer_new(&ctx.decl, sizeof(buffer));
    map_new_string(&ctx.decln, sizeof(size_t), NULL, NULL, NULL, NULL);

    ret = sc(file_read_whole(PREAMBLE_PATH));
    snewln(&ret);
    saddln(&ret, emit_program(&ctx, IIR_BEGIN));
    saddln(&ret, sc(file_read_whole(EPILOGUE_PATH)));

    return sget(ret);
}
