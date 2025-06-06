#include "emit.h"
#include <files.h>
#include <string.h>

#define EMIT(X, N) emit_##X(ctx, GET_IRID(N))
#define EMITT(X, N) emit_##X(ctx, GET_IRID(N), GET_IRTYPE(N))
#define EMITTX(X, N, Y) emit_##X(ctx, GET_IRID(N), GET_IRTYPE(N), Y)
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

static string get_functor(Ctx *ctx, iIR struct_def, const char *functor) {
    Struct *obj;
    iIR ifunc;
    obj = (Struct *)(TYPE(struct_def)->data.ptr);
    ifunc = *map_get(obj->methods, functor, iIR);
    return sc(*buffer_get(ctx->sem.mangling, ifunc, const char *));
}

static string rely_on_c_unary_left(const char *symbol, string expr) {
    string ret = sc(symbol);
    sadd(&ret, expr);
    return ret;
}

#define RELY_ON_C_UNARY_LEFT(SYM) rely_on_c_unary_left(SYM, expr.code)

static string rely_on_c_unary_right(const char *symbol, string expr) {
    string ret = expr;
    saddc(&ret, symbol);
    return ret;
}

#define RELY_ON_C_UNARY_RIGHT(SYM) rely_on_c_unary_right(SYM, expr.code)

static string rely_on_c_binary(const char *symbol, string lhs, string rhs) {
    string ret = lhs;
    saddc(&ret, " ");
    saddc(&ret, symbol);
    saddc(&ret, " ");
    sadd(&ret, rhs);
    return ret;
}

#define RELY_ON_C_BINARY(SYM) rely_on_c_binary(SYM, lhs.code, rhs.code)

static string functor_unary(
    Ctx *ctx, iIR struct_def, const char *functor, string expr) {
    string ret = get_functor(ctx, struct_def, functor);
    saddc(&ret, "(&");
    sadd(&ret, expr);
    saddc(&ret, ")");
    return ret;
}

#define FUNCTOR_UNARY(FUNCTOR)                                                 \
    functor_unary(ctx, type.data.word, FUNCTOR, expr.code)

static string functor_binary(
    Ctx *ctx, iIR struct_def, const char *functor, string lhs, string rhs) {
    string ret = get_functor(ctx, struct_def, functor);
    saddc(&ret, "(&");
    sadd(&ret, lhs);
    saddc(&ret, ", ");
    sadd(&ret, rhs);
    saddc(&ret, ")");
    return ret;
}

#define FUNCTOR_BINARY(FUNCTOR)                                                \
    functor_binary(ctx, lhst.data.word, FUNCTOR, lhs.code, rhs.code)

static Expr __bool__(Ctx *ctx, Expr expr, Type type) {
    Expr ret;

    ret.above = buffer_copy(expr.above);
    ret.self_val = snew();
    ret.lvalue = 0;

    switch (type.id) {
    case TYPE_BOOL:
    case TYPE_BYTE:
    case TYPE_FLOAT:
    case TYPE_PTR:
    case TYPE_STRING:
    case TYPE_WORD:
        /* Rely on C */
        ret.code = expr.code;
        break;
    case TYPE_STRUCT_INST:
        ret.code = FUNCTOR_UNARY("__bool__");
        break;
    default:
        UNREACHABLE;
    }

    return ret;
}

static Expr _arith_unary(Ctx *ctx, Expr expr, Type type, const char *functor,
    const char *c, bool left) {
    Expr ret;

    ret.above = buffer_copy(expr.above);
    ret.self_val = snew();
    ret.lvalue = 0;

    switch (type.id) {
    case TYPE_BYTE:
    case TYPE_FLOAT:
    case TYPE_WORD:
        if (left)
            ret.code = RELY_ON_C_UNARY_LEFT(c);
        else
            ret.code = RELY_ON_C_UNARY_RIGHT(c);
        break;
    case TYPE_STRING:
        todo();
        break;
    case TYPE_STRUCT_INST:
        ret.code = FUNCTOR_UNARY(functor);
        break;
    default:
        throwe("`%s' is undefined for the given type", functor);
    }

    return ret;
}

#define __incr__(LHS, LHST)                                                    \
    _arith_unary(ctx, LHS, LHST, "__incr__", "++", false)
#define __incl__(LHS, LHST) _arith_unary(ctx, LHS, LHST, "__incl__", "++", true)
#define __decr__(LHS, LHST)                                                    \
    _arith_unary(ctx, LHS, LHST, "__decr__", "--", false)
#define __decl__(LHS, LHST) _arith_unary(ctx, LHS, LHST, "__decl__", "--", true)

static Expr _arith(Ctx *ctx, Expr lhs, Type lhst, Expr rhs, const char *functor,
    const char *c) {
    Expr ret;

    ret.above = buffer_copy(lhs.above);
    buffer_join(ret.above, rhs.above);
    ret.self_val = snew();
    ret.lvalue = 0;

    switch (lhst.id) {
    case TYPE_BYTE:
    case TYPE_FLOAT:
    case TYPE_WORD:
        ret.code = RELY_ON_C_BINARY(c);
        break;
    case TYPE_STRING:
        todo();
        break;
    case TYPE_STRUCT_INST:
        ret.code = FUNCTOR_BINARY(functor);
        break;
    default:
        UNREACHABLE;
    }

    return ret;
}

#define __star__(LHS, LHST, RHS) _arith(ctx, LHS, LHST, RHS, "__star__", "*")
#define __slash__(LHS, LHST, RHS) _arith(ctx, LHS, LHST, RHS, "__slash__", "/")
#define __perc__(LHS, LHST, RHS) _arith(ctx, LHS, LHST, RHS, "__perc__", "%")
#define __plus__(LHS, LHST, RHS) _arith(ctx, LHS, LHST, RHS, "__plus__", "+")
#define __minus__(LHS, LHST, RHS) _arith(ctx, LHS, LHST, RHS, "__minus__", "-")

static Expr _dstar(Ctx *ctx, Expr lhs, Type lhst, Expr rhs) {
    Expr ret;

    ret.above = buffer_copy(lhs.above);
    buffer_join(ret.above, rhs.above);
    ret.self_val = snew();
    ret.lvalue = 0;

    switch (lhst.id) {
    case TYPE_BYTE:
    case TYPE_FLOAT:
    case TYPE_WORD:
        /* preamble: pow_byte, pow_float, pow_word */
        todo();
        break;
    case TYPE_STRING:
        todo();
        break;
    case TYPE_STRUCT_INST:
        ret.code = FUNCTOR_BINARY("__dstar__");
        break;
    default:
        UNREACHABLE;
    }

    return ret;
}

#define __dstar__(LHS, LHST, RHS) _dstar(ctx, LHS, LHST, RHS)

static Expr _tilde(Ctx *ctx, Expr expr, Type type) {
    Expr ret;

    ret.above = buffer_copy(expr.above);
    ret.self_val = snew();
    ret.lvalue = 0;

    switch (type.id) {
    case TYPE_BYTE:
    case TYPE_WORD:
        ret.code = RELY_ON_C_UNARY_LEFT("~");
        break;
    case TYPE_STRUCT_INST:
        ret.code = FUNCTOR_UNARY("__tilde__");
        break;
    default:
        UNREACHABLE;
    }

    return ret;
}

#define __tilde__(EXPR, TYPE) _tilde(ctx, EXPR, TYPE)

static Expr _bitwise_binary(Ctx *ctx, Expr lhs, Type lhst, Expr rhs,
    const char *functor, const char *c) {
    Expr ret;

    ret.above = buffer_copy(lhs.above);
    buffer_join(ret.above, rhs.above);
    ret.self_val = snew();
    ret.lvalue = 0;

    switch (lhst.id) {
    case TYPE_BYTE:
    case TYPE_WORD:
        ret.code = RELY_ON_C_BINARY(c);
        break;
    case TYPE_STRUCT_INST:
        ret.code = FUNCTOR_BINARY(functor);
        break;
    default:
        UNREACHABLE;
    }

    return ret;
}

#define __hat__(LHS, LHST, RHS)                                                \
    _bitwise_binary(ctx, LHS, LHST, RHS, "__hat__", "^")
#define __amp__(LHS, LHST, RHS)                                                \
    _bitwise_binary(ctx, LHS, LHST, RHS, "__amp__", "&")
#define __bar__(LHS, LHST, RHS)                                                \
    _bitwise_binary(ctx, LHS, LHST, RHS, "__bar__", "|")

static Expr _comp_eq(Ctx *ctx, Expr lhs, Type lhst, Expr rhs) {
    Expr ret;

    ret.above = buffer_copy(lhs.above);
    buffer_join(ret.above, rhs.above);
    ret.self_val = snew();
    ret.lvalue = 0;

    switch (lhst.id) {
    case TYPE_BOOL:
    case TYPE_BYTE:
    case TYPE_FLOAT:
    case TYPE_PTR:
    case TYPE_WORD:
        ret.code = RELY_ON_C_BINARY("==");
        break;
    case TYPE_STRING:
        todo();
        break;
    case TYPE_TUPLE:
        todo();
        break;
    case TYPE_STRUCT_INST:
        ret.code = FUNCTOR_BINARY("__eq__");
        break;
    default:
        UNREACHABLE;
    }

    return ret;
}

#define __eq__(LHS, LHST, RHS) _comp_eq(ctx, LHS, LHST, RHS)

static Expr _comp_rest(Ctx *ctx, Expr lhs, Type lhst, Expr rhs,
    const char *functor, const char *c) {
    Expr ret;

    ret.above = buffer_copy(lhs.above);
    buffer_join(ret.above, rhs.above);
    ret.self_val = snew();
    ret.lvalue = 0;

    switch (lhst.id) {
    case TYPE_BOOL:
    case TYPE_BYTE:
    case TYPE_FLOAT:
    case TYPE_WORD:
        ret.code = RELY_ON_C_BINARY(c);
        break;
    case TYPE_STRING:
        todo();
        break;
    case TYPE_STRUCT_INST:
        ret.code = FUNCTOR_BINARY(functor);
        break;
    default:
        UNREACHABLE;
    }

    return ret;
}

#define __neq__(LHS, LHST, RHS) _comp_rest(ctx, LHS, LHST, RHS, "__neq__", "!=")
#define __lt__(LHS, LHST, RHS) _comp_rest(ctx, LHS, LHST, RHS, "__lt__", "<")
#define __leq__(LHS, LHST, RHS) _comp_rest(ctx, LHS, LHST, RHS, "__leq__", "<=")
#define __gt__(LHS, LHST, RHS) _comp_rest(ctx, LHS, LHST, RHS, "__gt__", ">")
#define __geq__(LHS, LHST, RHS) _comp_rest(ctx, LHS, LHST, RHS, "__geq__", ">=")

static Expr _assign_eq(Ctx *ctx, Expr lhs, Type lhst, Expr rhs) {
    Expr ret;

    ret.above = buffer_copy(lhs.above);
    buffer_join(ret.above, rhs.above);
    ret.self_val = snew();
    ret.lvalue = 0;

    switch (lhst.id) {
    case TYPE_BOOL:
    case TYPE_BYTE:
    case TYPE_FLOAT:
    case TYPE_PTR:
    case TYPE_WORD:
        ret.code = RELY_ON_C_BINARY("=");
        break;
    case TYPE_STRING:
        todo();
        break;
    case TYPE_TUPLE:
        todo();
        break;
    case TYPE_STRUCT_INST:
        ret.code = FUNCTOR_BINARY("__assign__");
        break;
    default:
        UNREACHABLE;
    }

    return ret;
}

#define __assign__(LHS, LHST, RHS) _assign_eq(ctx, LHS, LHST, RHS)
#define __assign_plus__(LHS, LHST, RHS)                                        \
    _arith(ctx, LHS, LHST, RHS, "__assign_plus__", "+=")
#define __assign_minus__(LHS, LHST, RHS)                                       \
    _arith(ctx, LHS, LHST, RHS, "__assign_minus__", "-=")
#define __assign_star__(LHS, LHST, RHS)                                        \
    _arith(ctx, LHS, LHST, RHS, "__assign_star__", "*=")
#define __assign_slash__(LHS, LHST, RHS)                                       \
    _arith(ctx, LHS, LHST, RHS, "__assign_slash__", "/=")
#define __assign_perc__(LHS, LHST, RHS)                                        \
    _arith(ctx, LHS, LHST, RHS, "__assign_perc__", "%=")
#define __assign_hat__(LHS, LHST, RHS)                                         \
    _arith(ctx, LHS, LHST, RHS, "__assign_hat__", "^=")
#define __assign_amp__(LHS, LHST, RHS)                                         \
    _arith(ctx, LHS, LHST, RHS, "__assign_amp__", "&=")
#define __assign_bar__(LHS, LHST, RHS)                                         \
    _arith(ctx, LHS, LHST, RHS, "__assign_bar__", "|=")

static Expr _assign_dstar(Ctx *ctx, Expr lhs, Type lhst, Expr rhs) {
    Expr ret;

    ret.above = buffer_copy(lhs.above);
    buffer_join(ret.above, rhs.above);
    ret.self_val = snew();
    ret.lvalue = 0;

    switch (lhst.id) {
    case TYPE_BYTE:
    case TYPE_FLOAT:
    case TYPE_WORD:
        /* preamble: pow_byte, pow_float, pow_word */
        todo();
        break;
    case TYPE_STRING:
        todo();
        break;
    case TYPE_STRUCT_INST:
        ret.code = FUNCTOR_BINARY("__assign_dstar__");
        break;
    default:
        UNREACHABLE;
    }

    return ret;
}

#define __assign_dstar__(LHS, LHST, RHS) _assign_dstar(ctx, LHS, LHST, RHS)

static Expr _question(Ctx *ctx, Expr expr, Type type) {
    Expr ret;

    ret.above = buffer_copy(expr.above);
    ret.self_val = snew();
    ret.lvalue = 0;

    switch (type.id) {
    case TYPE_BOOL:
    case TYPE_BYTE:
    case TYPE_FLOAT:
    case TYPE_PTR:
    case TYPE_WORD: {
        string assertion = sc("_Xassert(");
        /* These types all have bool downcast */
        sadd(&assertion, expr.code);
        saddc(&assertion, ");");

        ret.code = expr.code;
        buffer_push(ret.above, &assertion);
        break;
    }
    case TYPE_STRING:
    case TYPE_TUPLE:
        todo();
        break;
    case TYPE_STRUCT_INST:
        ret.code = FUNCTOR_UNARY("__question__");
        break;
    default:
        throw("`__question__' is undefined for the given type");
    }

    return ret;
}

#define __question__(EXPR, TYPE) _question(ctx, EXPR, TYPE)

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
    Expr sub, sub2;
    IR *ir = GET_IR(iir);
    sub = EMITT(expr, 0);
    sub2 = EMITT(expr, 2);

    switch (type) {
    case IR_assign_eq:
        return __assign__(sub, *TYPE_CHILD(0), sub2);
    case IR_assign_plus:
        return __assign_plus__(sub, *TYPE_CHILD(0), sub2);
    case IR_assign_minus:
        return __assign_minus__(sub, *TYPE_CHILD(0), sub2);
    case IR_assign_dstar:
        return __assign_dstar__(sub, *TYPE_CHILD(0), sub2);
    case IR_assign_star:
        return __assign_star__(sub, *TYPE_CHILD(0), sub2);
    case IR_assign_slash:
        return __assign_slash__(sub, *TYPE_CHILD(0), sub2);
    case IR_assign_perc:
        return __assign_perc__(sub, *TYPE_CHILD(0), sub2);
    case IR_assign_hat:
        return __assign_hat__(sub, *TYPE_CHILD(0), sub2);
    case IR_assign_amp:
        return __assign_amp__(sub, *TYPE_CHILD(0), sub2);
    case IR_assign_bar:
        return __assign_bar__(sub, *TYPE_CHILD(0), sub2);
    default:
        UNREACHABLE;
    }

    return sub;
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
    case IR_expr_at:
        assert(*buffer_get(ctx->sem.mangling, iir, void *));
        ret.code = sc(*buffer_get(ctx->sem.mangling, iir, const char *));
        ret.self_val = ret.code;
        ret.lvalue = 1;
        break;
    case IR_expr_self:
        ret.code = sc("*self"); /* TODO: might be reference! */
        ret.lvalue = 1;
        break;
    case IR_expr_local: {
        string name;
        Struct *obj;

        name = sc(get_id(ctx, GET_IRID(1)));
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
    case IR_expr_nothing:
        /* Nothing to do here, relies on context */
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
            /* TODO: `&' hardcoded in self_val is so cursed */
            assert(sub.self_val.len);
            sadd(&ret.code, sub.self_val);
            saddc(&ret.code, ", ");
        }
        sub = EMITT(expr_list, 2);
        sadd(&ret.code, sub.code);
        buffer_join(ret.above, sub.above);
        saddc(&ret.code, ")");

        /* TODO: This is terrible.
           This should happen at a later point,
           when a reference is needed and it's an rvalue */
        if (TYPE(iir)->id == TYPE_STRUCT_INST) {
            iIR base;
            string struct_name, name, assignment;

            base = TYPE(iir)->data.word;
            struct_name =
                sc(*buffer_get(ctx->sem.mangling, base, const char *));
            name = sc("_Xinst");
            countit(ctx, &name);
            push_decl(ctx, struct_name, name, snew());

            assignment = sdup(name);
            saddc(&assignment, " = ");
            sadd(&assignment, ret.code);
            saddc(&assignment, ";");
            buffer_push(ret.above, &assignment);
            ret.code = name;
        }

        break;
    case IR_expr_lit:
        sadd(&ret.code, EMITT(lit, 0));
        break;
    case IR_expr_question:
        sub = EMITT(expr, 0);
        ret = __question__(sub, *TYPE_CHILD(0));
        break;
    case IR_expr_inc_right:
        sub = EMITT(expr, 0);
        ret = __incr__(sub, *TYPE_CHILD(0));
        break;
    case IR_expr_inc_left:
        sub = EMITT(expr, 1);
        ret = __incl__(sub, *TYPE_CHILD(1));
        break;
    case IR_expr_dec_right:
        sub = EMITT(expr, 0);
        ret = __decr__(sub, *TYPE_CHILD(0));
        break;
    case IR_expr_dec_left:
        sub = EMITT(expr, 1);
        ret = __decl__(sub, *TYPE_CHILD(1));
        break;
    case IR_expr_sizeof:
    case IR_expr_tilde:
        sub = EMITT(expr, 1);
        ret = __tilde__(sub, *TYPE_CHILD(1));
        break;
    case IR_expr_hat:
        sub = EMITT(expr, 0);
        sub2 = EMITT(expr, 2);
        ret = __hat__(sub, *TYPE_CHILD(0), sub2);
        break;
    case IR_expr_amp:
        sub = EMITT(expr, 0);
        sub2 = EMITT(expr, 2);
        ret = __amp__(sub, *TYPE_CHILD(0), sub2);
        break;
    case IR_expr_bar:
        sub = EMITT(expr, 0);
        sub2 = EMITT(expr, 2);
        ret = __bar__(sub, *TYPE_CHILD(0), sub2);
        break;
    case IR_expr_dstar:
        sub = EMITT(expr, 0);
        sub2 = EMITT(expr, 2);
        ret = __dstar__(sub, *TYPE_CHILD(0), sub2);
        break;
    case IR_expr_star:
        sub = EMITT(expr, 0);
        sub2 = EMITT(expr, 2);
        ret = __star__(sub, *TYPE_CHILD(0), sub2);
        break;
    case IR_expr_slash:
        sub = EMITT(expr, 0);
        sub2 = EMITT(expr, 2);
        ret = __slash__(sub, *TYPE_CHILD(0), sub2);
        break;
    case IR_expr_perc:
        sub = EMITT(expr, 0);
        sub2 = EMITT(expr, 2);
        ret = __perc__(sub, *TYPE_CHILD(0), sub2);
        break;
    case IR_expr_plus:
        sub = EMITT(expr, 0);
        sub2 = EMITT(expr, 2);
        ret = __plus__(sub, *TYPE_CHILD(0), sub2);
        break;
    case IR_expr_minus:
        sub = EMITT(expr, 0);
        sub2 = EMITT(expr, 2);
        ret = __minus__(sub, *TYPE_CHILD(0), sub2);
        break;
    case IR_expr_deq:
        sub = EMITT(expr, 0);
        sub2 = EMITT(expr, 2);
        ret = __eq__(sub, *TYPE_CHILD(0), sub2);
        break;
    case IR_expr_neq:
        sub = EMITT(expr, 0);
        sub2 = EMITT(expr, 2);
        ret = __neq__(sub, *TYPE_CHILD(0), sub2);
        break;
    case IR_expr_lt:
        sub = EMITT(expr, 0);
        sub2 = EMITT(expr, 2);
        ret = __lt__(sub, *TYPE_CHILD(0), sub2);
        break;
    case IR_expr_leq:
        sub = EMITT(expr, 0);
        sub2 = EMITT(expr, 2);
        ret = __leq__(sub, *TYPE_CHILD(0), sub2);
        break;
    case IR_expr_gt:
        sub = EMITT(expr, 0);
        sub2 = EMITT(expr, 2);
        ret = __gt__(sub, *TYPE_CHILD(0), sub2);
        break;
    case IR_expr_geq:
        sub = EMITT(expr, 0);
        sub2 = EMITT(expr, 2);
        ret = __geq__(sub, *TYPE_CHILD(0), sub2);
        break;
    case IR_expr_not:
        saddc(&ret.code, "!");
        sub = EMITT(expr, 1);
        sub2 = __bool__(ctx, sub, *TYPE_CHILD(1));
        sadd(&ret.code, sub2.code);
        ret.above = sub2.above;
        break;
    case IR_expr_and:
        sub = EMITT(expr, 0);
        sub2 = __bool__(ctx, sub, *TYPE_CHILD(0));
        sadd(&ret.code, sub2.code);
        ret.above = sub2.above;
        saddc(&ret.code, " && ");
        sub = EMITT(expr, 2);
        sub2 = __bool__(ctx, sub, *TYPE_CHILD(2));
        sadd(&ret.code, sub2.code);
        buffer_join(ret.above, sub2.above);
        break;
    case IR_expr_or:
        sub = EMITT(expr, 0);
        sub2 = __bool__(ctx, sub, *TYPE_CHILD(0));
        sadd(&ret.code, sub2.code);
        ret.above = sub2.above;
        saddc(&ret.code, " || ");
        sub = EMITT(expr, 2);
        sub2 = __bool__(ctx, sub, *TYPE_CHILD(2));
        sadd(&ret.code, sub2.code);
        buffer_join(ret.above, sub2.above);
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

static string emit_block(Ctx *ctx, iIR iir, IRType type, bool loop);
static string emit_if(Ctx *ctx, iIR iir, IRType type) {
    string above = snew();
    string contents;
    IR *ir = GET_IR(iir);
    Expr cond;
    size_t i;

    cond = EMITT(expr, 1);
    for (i = 0; i < buffer_num(cond.above); ++i)
        saddln(&above, *buffer_get(cond.above, i, string));

    contents = sc("if (");
    sadd(&contents, cond.code);
    saddc(&contents, ") ");
    sadd(&contents, EMITTX(block, 2, false));
    switch (type) {
    case IR_if_only:
        /* Nothing to do */
        break;
    case IR_if_elif:
        iir = GET_IRID(3);
        type = GET_IRTYPE(3);
        for (;;) {
            ir = GET_IR(iir);
            switch (type) {
            case IR_elif_only:
                cond = EMITT(expr, 1);
                for (i = 0; i < buffer_num(cond.above); ++i)
                    saddln(&above, *buffer_get(cond.above, i, string));

                saddc(&contents, "else if (");
                sadd(&contents, cond.code);
                saddc(&contents, ") ");
                sadd(&contents, EMITTX(block, 2, false));
                goto end;
            case IR_elif_rec:
                cond = EMITT(expr, 1);
                for (i = 0; i < buffer_num(cond.above); ++i)
                    saddln(&above, *buffer_get(cond.above, i, string));

                saddc(&contents, "else if (");
                sadd(&contents, cond.code);
                saddc(&contents, ") ");
                sadd(&contents, EMITTX(block, 2, false));
                iir = GET_IRID(3);
                type = GET_IRTYPE(3);
                break;
            case IR_elif_else:
                iir = GET_IRID(0);
                assert(GET_IRTYPE(0) == IR_else);
                ir = GET_IR(iir);
                saddc(&contents, "else ");
                sadd(&contents, EMITTX(block, 1, false));
                goto end;
            default:
                UNREACHABLE;
            }
        }
        break;
    default:
        UNREACHABLE;
    }

end:
    sadd(&above, contents);
    return above;
}

static string emit_stmt(Ctx *ctx, iIR iir, IRType type) {
    string ret = snew();
    IR *ir = GET_IR(iir);

    switch (type) {
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
        Expr expr, cast;
        size_t i;

        expr = EMITT(expr, 1);
        cast = __bool__(ctx, expr, *TYPE_CHILD(1));
        for (i = 0; i < buffer_num(cast.above); ++i)
            saddln(&ret, *buffer_get(cast.above, i, string));

        saddc(&ret, "_Xassert(");
        sadd(&ret, cast.code);
        saddlnc(&ret, ");");
        break;
    }
    case IR_stmt_break:
        saddc(&ret, "goto loop");
        saddi(&ret, *buffer_back(ctx->loops, size_t));
        saddlnc(&ret, "_exit;");
        break;
    case IR_stmt_continue:
        saddc(&ret, "goto loop");
        saddi(&ret, *buffer_back(ctx->loops, size_t));
        saddlnc(&ret, "_next;");
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

        if (IS_INFER(iir)) {
            Expr aux;
            Type expected;
            Struct *obj;

            aux.above = expr.above;
            expected = *(Type *)(TYPE(iir)->data.ptr);
            free(TYPE(iir)->data.ptr);
            if (expected.id == TYPE_STRUCT_INST)
                expected = *TYPE(expected.data.word);
            obj = (Struct *)(expected.data.ptr);

            if (TYPE_CHILD(1)->id == TYPE_NONE) {
                iIR inone;
                inone = *map_get(obj->methods, "__none__", iIR);
                aux.code =
                    sc(*buffer_get(ctx->sem.mangling, inone, const char *));
                saddc(&aux.code, "()");
            } else {
                iIR ifrom;
                ifrom = *map_get(obj->methods, "__from__", iIR);
                aux.code =
                    sc(*buffer_get(ctx->sem.mangling, ifrom, const char *));
                saddc(&aux.code, "(");
                sadd(&aux.code, expr.code);
                saddc(&aux.code, ")");
            }

            expr = aux;
        }

        for (i = 0; i < buffer_num(expr.above); ++i)
            saddln(&ret, *buffer_get(expr.above, i, string));
        saddc(&ret, "return ");
        sadd(&ret, expr.code);
        saddlnc(&ret, ";");
        break;
    }
    case IR_stmt_if:
        saddln(&ret, EMITT(if, 0));
        break;
    case IR_stmt_switch:
        todo();
        break;
    case IR_stmt_for:
        todo();
        break;
    case IR_stmt_while: {
        Expr expr = EMITT(expr, 1);
        size_t i;
        for (i = 0; i < buffer_num(expr.above); ++i)
            saddln(&ret, *buffer_get(expr.above, i, string));
        saddc(&ret, "while (");
        sadd(&ret, expr.code);
        saddc(&ret, ") ");

        i = ctx->loop_ctr++; /* recycle variable */
        buffer_push(ctx->loops, &i);
        sadd(&ret, EMITTX(block, 2, true));
        buffer_pop(ctx->loops);
        snewln(&ret);
        break;
    }
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

static string emit_block(Ctx *ctx, iIR iir, IRType type, bool loop) {
    string ret = sc("{");
    IR *ir = GET_IR(iir);
    string body;
    buffer decls;
    size_t i;

    snewln(&ret);
    push_decls(ctx);
    switch (type) {
    case IR_block_braces:
        body = EMITT(stmts, 1);
        break;
    case IR_block_stmt:
        body = EMITT(stmt, 0);
        break;
    default:
        UNREACHABLE;
    }
    decls = pop_decls(ctx);

    for (i = 0; i < buffer_num(decls); ++i)
        saddln(&ret, *buffer_get(decls, i, string));
    if (i)
        snewln(&ret);
    sadd(&ret, body);

    if (loop) {
        size_t i = *buffer_back(ctx->loops, size_t);

        saddc(&ret, "loop");
        saddi(&ret, i);
        saddlnc(&ret, "_next:");
        /* TODO: destructors here */
        saddlnc(&ret, "continue;");

        saddc(&ret, "loop");
        saddi(&ret, i);
        saddlnc(&ret, "_exit:");
        /* TODO: destructors here too */
        saddlnc(&ret, "break;");
    } else {
        /* Destructors here */
    }

    saddc(&ret, "}");

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
            if (!IS_MUTABLE(iir))
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
            saddc(&ret, "(");
            if (!IS_MUTABLE(iir))
                saddc(&ret, "const ");
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

        /* TODO: consider destructors? */

        /* Return */
        if (func->ret.id != TYPE_NOTHING)
            saddc(&ret, "return ");
        sadd(&ret, expr.code);
        saddlnc(&ret, ";");

        saddc(&ret, "}");
    } else {
        sadd(&ret, EMITTX(block, block, false));
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
    buffer_new(&ctx.decl, sizeof(buffer));
    ctx.decln = NULL;
    map_new_string(&ctx.decln, sizeof(size_t), NULL, NULL, NULL, NULL);
    ctx.self_struct = 0;
    ctx.loop_ctr = 0;
    ctx.loops = NULL;
    buffer_new(&ctx.loops, sizeof(size_t));

    ret = sc(file_read_whole(PREAMBLE_PATH));
    snewln(&ret);
    saddln(&ret, emit_program(&ctx, IIR_BEGIN));
    saddln(&ret, sc(file_read_whole(EPILOGUE_PATH)));

    return sget(ret);
}
