#include "semantics.h"

#define GET_IR(N) (buffer_get(ctx->irs, N, IR))
#define GET_IRTYPE(N) (*buffer_get(ir->types, N, IRType))
#define GET_IRID(N) (*buffer_get(ir->ids, N, iIR))
#define GET_TOKEN(N) (buffer_get(ctx->tokens, N, Token))

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

/* --- */

static iIR lookup(Ctx *ctx, const char *name) {
    SymbolTable table;
    size_t i;
    size_t num;

    table = *buffer_get(ctx->tables, ctx->current, SymbolTable);
    num = buffer_num(table);
    for (i = 0; i < num; ++i) {
        Symbols syms = *buffer_get(table, num - i - 1, Symbols);
        if (map_has(syms, name))
            return *map_get(syms, name, iIR);
    }

    throwe("could not find: %s", name);
    return 0;
}

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

static void sem_primary(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);

    switch (type) {
    case IR_primary_dot:
        todo();
        break;
    case IR_primary_typed:
        todo();
        break;
    case IR_primary_id: {
        const char *name;
        iIR e;
        name = get_id(ctx, GET_IRID(0));
        e = lookup(ctx, name);
        buffer_set(ctx->mangling, iir, buffer_get(ctx->mangling, e, void));
        *TYPE(iir) = *TYPE(e);
        break;
    }
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
        SEMT(primary, 0);
        COPY_TYPE(0);
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

static void sem_decl_nonglobal(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);
    const char *name;
    char *mangled;

    switch (type) {
    case IR_decl_id:
        assert(GET_IRTYPE(1) == IR_TOKEN);
        name = get_id(ctx, GET_IRID(1));
        mangled = mangle_var(ctx, name);
        buffer_set(ctx->mangling, iir, &mangled);
        SEMT(expr, 3);
        COPY_TYPE(3);
        break;
    case IR_decl_p_id:
        todo();
        break;
    case IR_decl_typed:
        todo();
        break;
    case IR_decl_p_typed:
        todo();
        break;
    default:
        UNREACHABLE;
    }

    push_to_scope(ctx, name, iir);
}

static void sem_stmt(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);

    switch (type) {
    case IR_stmt_block:
        todo();
        break;
    case IR_stmt_decl:
        SEMT(decl_nonglobal, 0);
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
        if (buffer_back(ctx->funcrets, Type)->id != TYPE_NOTHING)
            throw("tried to return nothing, expected something");
        break;
    case IR_stmt_retval:
        SEMT(expr, 1);
        if (!check_types(*TYPE_CHILD(1), *buffer_back(ctx->funcrets, Type)))
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

static const char *sem_typed_id(Ctx *ctx, iIR iir) {
    IR *ir = GET_IR(iir);
    ASSERT_CHILDREN(3);

    /* Resolve type and copy */
    SEMT(type, 2);
    COPY_TYPE(2);

    /* Return id */
    assert(GET_IRTYPE(0) == IR_TOKEN);
    return get_id(ctx, GET_IRID(0));
}

/* PARAM, but in functions (pushes symbols to the scope) */
static void sem_func_param(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);
    size_t where = 0;
    const char *name;

    switch (type) {
    case IR_param_copy:
        where = 0;
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

    /* Analyze now the TYPED_ID */
    name = SEM(typed_id, where);
    COPY_TYPE(where);
    buffer_set(ctx->mangling, iir, &name);
    push_to_scope(ctx, name, iir);
}

/* PARAMS, but in functions (pushes symbols to the scope) */
static void sem_func_params(Ctx *ctx, iIR iir, IRType type) {
    for (;;) {
        IR *ir = GET_IR(iir);
        switch (type) {
        case IR_params_rec:
            SEMT(func_param, 0);
            iir = GET_IRID(1);
            type = GET_IRTYPE(1);
            break;
        case IR_params_one:
            SEMT(func_param, 0);
            return;
        default:
            UNREACHABLE;
        }
    }
}

static void sem_func(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);
    const char *name;
    Type funcret;
    char *mangled;
    size_t block;

    /* TODO: annotations */

    assert(GET_IRTYPE(2) == IR_TOKEN);
    name = get_id(ctx, GET_IRID(2));

    push_scope(ctx);

    switch (type) {
    case IR_function_noargs_void:
        funcret.id = TYPE_NOTHING;
        funcret.data.nothing = NULL;
        block = 3;
        break;
    case IR_function_noargs_typed:
        SEMT(type, 4);
        funcret = *TYPE_CHILD(4);
        block = 5;
        break;
    case IR_function_void:
        todo();
        funcret.id = TYPE_NOTHING;
        funcret.data.nothing = NULL;
        block = 6;
        break;
    case IR_function_typed:
        SEMT(type, 7);
        SEMT(func_params, 4);
        funcret = *TYPE_CHILD(7);
        block = 8;
        break;
    default:
        UNREACHABLE;
    }

    /* New funcrets item */
    buffer_push(ctx->funcrets, &funcret);

    mangled = mangle(ctx, name);
    buffer_set(ctx->mangling, iir, &mangled);

    SEM(block, block);

    buffer_pop(ctx->funcrets);
    pop_scope(ctx);
}

static void sem_decl_global(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);
    const char *name;
    char *mangled;

    switch (type) {
    case IR_decl_id:
        assert(GET_IRTYPE(1) == IR_TOKEN);
        name = get_id(ctx, GET_IRID(1));
        if (in_scope(ctx, name))
            throwe("already declared: %s", name);
        mangled = mangle(ctx, name);
        buffer_set(ctx->mangling, iir, &mangled);
        SEMT(expr, 3);
        COPY_TYPE(3);
        break;
    case IR_decl_p_id:
        todo();
        break;
    case IR_decl_typed:
        todo();
        break;
    case IR_decl_p_typed:
        todo();
        break;
    default:
        UNREACHABLE;
    }

    push_to_scope(ctx, name, iir);
}

static void sem_global(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);

    switch (type) {
    case IR_global_decl:
        SEMT(decl_global, 0);
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

    /* Symbol table for the program */
    ctx->current = iir;
    do {
        SymbolTable *st;
        st = buffer_get(ctx->tables, ctx->current, SymbolTable);
        buffer_new(st, sizeof(Symbols));
    } while (0);

    push_scope(ctx);

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

    /* Augmented context: types */
    ctx.types = NULL;
    buffer_new(&ctx.types, sizeof(Type));
    buffer_zresize(ctx.types, buffer_num(ctx.irs));
    /* Augmented context: name mangling */
    ctx.mangling = NULL;
    buffer_new(&ctx.mangling, sizeof(const char *));
    buffer_zresize(ctx.mangling, buffer_num(ctx.irs));

    /* State: symbol tables */
    ctx.tables = NULL;
    buffer_new(&ctx.tables, sizeof(SymbolTable));
    buffer_zresize(ctx.tables, buffer_num(ctx.irs));
    /* State: module stack */
    ctx.modstack = NULL;
    buffer_new(&ctx.modstack, sizeof(const char *));
    /* State: expected return type of called functions */
    ctx.funcrets = NULL;
    buffer_new(&ctx.funcrets, sizeof(Type));
    /* State: shadowing stack */
    ctx.shadowing = NULL;
    buffer_new(&ctx.shadowing, sizeof(ShadowingTable));

    sem_program(&ctx, buffer_num(ctx.irs) - 1);

    ret.types = ctx.types;
    ret.mangling = ctx.mangling;
    return ret;
}
