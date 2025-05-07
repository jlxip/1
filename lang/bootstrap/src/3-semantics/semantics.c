#include "semantics.h"

#define ASSERT_CHILDREN(N) assert(buffer_num(ir->types) == N)
#define SEM(X, N) sem_##X(ctx, GET_IRID(N))
#define SEMT(X, N) sem_##X(ctx, GET_IRID(N), GET_IRTYPE(N))
#define TYPE(N) buffer_get(ctx->types, N, Type)
#define TYPE_CHILD(N) TYPE(GET_IRID(N))
#define COPY_TYPE(N)                                                           \
    do {                                                                       \
        assert(TYPE_CHILD(N)->id != TYPE_UNKNOWN);                             \
        *TYPE(iir) = *TYPE_CHILD(N);                                           \
    } while (0)

static bool check_types(Type a, Type b) {
    if (!a.id || !b.id)
        throw("TYPE_UNKNOWN! (bug)");

    if (a.id != b.id)
        return false;

    switch (a.id) {
    case TYPE_NOTHING:
    case TYPE_BOOL:
    case TYPE_BYTE:
    case TYPE_FLOAT:
    case TYPE_PTR:
    case TYPE_STRING:
    case TYPE_WORD:
        /* These are trivial */
        break;
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

    return true;
}

/* --- */

static const char *get_id(Ctx *ctx, iToken itoken) {
    Token *token = GET_TOKEN(itoken);
    assert(token->id == T_ID);
    return token->data.str;
}

/* --- */

static void sem_type(Ctx *ctx, iIR iir, IRType type) {
    switch (type) {
    case IR_type_bool:
        TYPE(iir)->id = TYPE_BOOL;
        TYPE(iir)->data.nothing = NULL;
        break;
    case IR_type_byte:
        TYPE(iir)->id = TYPE_BYTE;
        TYPE(iir)->data.nothing = NULL;
        break;
    case IR_type_float:
        TYPE(iir)->id = TYPE_FLOAT;
        TYPE(iir)->data.nothing = NULL;
        break;
    case IR_type_ptr:
        TYPE(iir)->id = TYPE_PTR;
        TYPE(iir)->data.nothing = NULL;
        break;
    case IR_type_string:
        TYPE(iir)->id = TYPE_STRING;
        TYPE(iir)->data.nothing = NULL;
        break;
    case IR_type_word:
        TYPE(iir)->id = TYPE_WORD;
        TYPE(iir)->data.nothing = NULL;
        break;
    case IR_type_direct:
        todo();
        break;
    case IR_type_tuple:
        todo();
        break;
    case IR_type_tuple_star:
        todo();
        break;
    default:
        UNREACHABLE;
    }
}

static void sem_lit(Ctx *ctx, iIR iir, IRType type) {
    switch (type) {
    case IR_lit_bool:
    case IR_lit_word:
        TYPE(iir)->id = TYPE_WORD;
        TYPE(iir)->data.nothing = NULL;
        break;
    case IR_lit_float:
    case IR_lit_string:
    case IR_lit_tuple:
        todo();
        break;
    default:
        UNREACHABLE;
    }
}

static void sem_expr(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);

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
        SEMT(lit, 0);
        COPY_TYPE(0);
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
}

static void sem_stmt(Ctx *ctx, iIR iir, IRType type) {
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
        SEMT(expr, 1);
        if (!check_types(*TYPE_CHILD(1), ctx->func->ret))
            throw("invalid return value type");
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
}

static void sem_stmts(Ctx *ctx, iIR iir, IRType type) {
    for (;;) {
        IR *ir = GET_IR(iir);

        switch (type) {
        case IR_stmts_rec:
            SEMT(stmt, 0);
            iir = GET_IRID(1);
            type = GET_IRTYPE(1);
            break;
        case IR_stmts_null:
            return;
        default:
            UNREACHABLE;
        }
    }
}

static void sem_block(Ctx *ctx, iIR iir) {
    IR *ir = GET_IR(iir);
    ASSERT_CHILDREN(3);
    SEMT(stmts, 1);
}

static void sem_func(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);
    const char *name;
    char *mangled;
    Function function;
    size_t block;

    if (!ctx->entry.functions)
        map_new_string(
            &ctx->entry.functions, sizeof(Function), NULL, NULL, NULL, NULL);

    /* TODO: annotations */

    assert(GET_IRTYPE(2) == IR_TOKEN);
    name = get_id(ctx, GET_IRID(2));

    switch (type) {
    case IR_function_noargs_void:
        function.args = NULL;
        function.ret.id = TYPE_NOTHING;
        function.ret.data.nothing = NULL;
        block = 3;
        break;
    case IR_function_noargs_typed:
        SEMT(type, 4);
        function.args = NULL;
        function.ret = *TYPE_CHILD(4);
        block = 5;
        break;
    case IR_function_void:
        todo();
        /*function.args = sem_params(ctx, GET_IRID(4));*/
        function.ret.id = TYPE_NOTHING;
        function.ret.data.nothing = NULL;
        block = 6;
        break;
    case IR_function_typed:
        SEMT(type, 7);
        todo();
        /*function.args = sem_params(ctx, GET_IRID(4));*/
        function.ret = *TYPE_CHILD(7);
        block = 8;
        break;
    default:
        UNREACHABLE;
    }

    map_add(ctx->entry.functions, name, &function); /* why?? no ST? */
    ctx->func = map_get(ctx->entry.functions, name, Function);

    mangled = mangle(ctx, name);
    buffer_set(ctx->mangling, GET_IRID(2), &mangled);

    SEM(block, block);
}

static void sem_global(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);

    switch (type) {
    case IR_global_decl:
        todo();
        break;
    case IR_global_func:
        SEMT(func, 0);
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
}

static void sem_globals(Ctx *ctx, iIR iir, IRType type) {
    for (;;) {
        IR *ir = GET_IR(iir);

        switch (type) {
        case IR_globals_rec:
            SEMT(global, 0);
            iir = GET_IRID(1);
            type = GET_IRTYPE(1);
            break;
        case IR_globals_null:
            return;
        default:
            UNREACHABLE;
        }
    }
}

static void sem_program(Ctx *ctx, iIR iir) {
    IR *ir = GET_IR(iir);
    ASSERT_CHILDREN(2);

    /* Uses */
    /* TODO */

    /* Globals */
    SEMT(globals, 1);
}

SemResult semantics(Tokens tokens, IRs irs) {
    Ctx ctx;
    SemResult ret;

    /* Given */
    ctx.tokens = tokens;
    ctx.irs = irs;

    /* Augmented context: types (of productions) */
    ctx.types = NULL;
    buffer_new(&ctx.types, sizeof(Type));
    buffer_zresize(ctx.types, buffer_num(ctx.irs));
    /* Augmented context: name mangling (of tokens) */
    ctx.mangling = NULL;
    buffer_new(&ctx.mangling, sizeof(const char *));
    buffer_zresize(ctx.mangling, buffer_num(ctx.tokens));

    /* State: functions */
    ctx.entry.functions = NULL;
    /* State: module stack */
    ctx.modstack = NULL;
    buffer_new(&ctx.modstack, sizeof(const char *));
    /* State: current function */
    ctx.func = NULL;

    sem_program(&ctx, buffer_num(ctx.irs) - 1);

    ret.types = ctx.types;
    ret.mangling = ctx.mangling;
    return ret;
}
