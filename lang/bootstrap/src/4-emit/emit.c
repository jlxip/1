#include "emit.h"
#include <ds/string.h>

#define EMIT(X, N) emit_##X(ctx, GET_IRID(N))
#define EMITT(X, N) emit_##X(ctx, GET_IRID(N), GET_IRTYPE(N))
#define TYPE(N) buffer_get(ctx->sem.types, N, Type)
#define TYPE_CHILD(N) TYPE(GET_IRID(N))

static const char *get_word(Ctx *ctx, iToken itoken) {
    Token *token = GET_TOKEN(itoken);
    assert(token->id == T_WORD);
    return token->data.str;
}

/* --- */

static string emit_type(Type type) {
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
        todo();
        break;
    case TYPE_WORD:
        return sc("size_t");
    case TYPE_TUPLE:
        todo();
        break;
    case TYPE_FUNC:
        UNREACHABLE;
        break;
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
    case IR_lit_word:
        saddc(&ret, get_word(ctx, GET_IRID(0)));
        break;
    case IR_lit_float:
    case IR_lit_string:
    case IR_lit_tuple:
        todo();
        break;
    default:
        UNREACHABLE;
    }

    return ret;
}

static string emit_expr(Ctx *ctx, iIR iir, IRType type) {
    string ret = snew();
    IR *ir = GET_IR(iir);

    saddc(&ret, "(");
    switch (type) {
    case IR_expr_par:
        todo();
        break;
    case IR_expr_primary:
        todo();
        break;
    case IR_expr_call:
        todo();
        break;
    case IR_expr_lit:
        sadd(&ret, EMITT(lit, 0));
        break;
    case IR_expr_sizeof:
        todo();
        break;
    case IR_expr_hat:
    case IR_expr_amp:
    case IR_expr_bar:
    case IR_expr_star:
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
    case IR_expr_struct_inst:
    case IR_expr_assign:
        todo();
        break;
    default:
        UNREACHABLE;
    }
    saddc(&ret, ")");

    return ret;
}

static string emit_stmt(Ctx *ctx, iIR iir, IRType type) {
    string ret = snew();
    IR *ir = GET_IR(iir);

    switch (type) {
    case IR_stmt_block:
        todo();
        break;
    case IR_stmt_decl:
        todo();
        break;
    case IR_stmt_expr:
        todo();
        break;
    case IR_stmt_assert:
        todo();
        break;
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
        todo();
        break;
    case IR_stmt_retval:
        saddc(&ret, "return ");
        sadd(&ret, EMITT(expr, 1));
        saddlnc(&ret, ";");
        break;
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

    snewln(&ret);
    sadd(&ret, EMITT(stmts, 1));
    saddlnc(&ret, "}");

    return ret;
}

static string emit_func(Ctx *ctx, iIR iir, IRType type) {
    string ret = snew();
    IR *ir = GET_IR(iir);
    const char *mangled;
    size_t block;

    mangled = *buffer_get(ctx->sem.mangling, GET_IRID(2), const char *);
    assert(mangled);

    switch (type) {
    case IR_function_noargs_void:
        todo();
        block = 3;
        break;
    case IR_function_noargs_typed:
        sadd(&ret, emit_type(*TYPE_CHILD(4)));
        saddc(&ret, " ");
        saddc(&ret, mangled);
        saddc(&ret, "(void) ");
        block = 5;
        break;
    case IR_function_void:
        todo();
        block = 6;
        break;
    case IR_function_typed:
        todo();
        block = 8;
        break;
    default:
        UNREACHABLE;
    }

    sadd(&ret, EMIT(block, block));
    return ret;
}

static string emit_global(Ctx *ctx, iIR iir, IRType type) {
    string ret;
    IR *ir = GET_IR(iir);

    switch (type) {
    case IR_global_decl:
        todo();
        break;
    case IR_global_func:
        ret = EMITT(func, 0);
        break;
    case IR_global_struct:
        todo();
        break;
    case IR_global_impl:
        todo();
        break;
    case IR_global_extern:
        todo();
        break;
    default:
        UNREACHABLE;
    }

    return ret;
}

static string emit_globals(Ctx *ctx, iIR iir, IRType type) {
    string ret = snew();

    for (;;) {
        IR *ir = GET_IR(iir);

        switch (type) {
        case IR_globals_rec:
            saddln(&ret, EMITT(global, 0));
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

char *emit(Tokens tokens, IRs irs, SemResult sem) {
    Ctx ctx;
    string ret;

    ctx.tokens = tokens;
    ctx.irs = irs;
    ctx.sem = sem;

    ret = sc("/* The 1 Programming Language */");
    snewln(&ret);
    snewln(&ret);
    saddlnc(&ret, "#include <stddef.h>");
    snewln(&ret);

    saddln(&ret, emit_program(&ctx, buffer_num(ctx.irs) - 1));

    return sget(ret);
}
