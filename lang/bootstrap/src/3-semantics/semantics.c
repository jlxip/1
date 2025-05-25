#include "semantics.h"
#include <ds/set.h>

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
    case TYPE_STRUCT_INST:
        if (a.data.word != b.data.word)
            return false;
        break;
    case TYPE_STRUCT_DEF:
    case TYPE_FUNC:
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

static iIR sem_primary(Ctx *ctx, iIR iir, IRType type);
static void sem_type(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);

    switch (type) {
    case IR_type_bool:
        TYPE(iir)->id = TYPE_BOOL;
        TYPE(iir)->data.ptr = NULL;
        TYPE(iir)->flags = 0;
        break;
    case IR_type_byte:
        TYPE(iir)->id = TYPE_BYTE;
        TYPE(iir)->data.ptr = NULL;
        TYPE(iir)->flags = 0;
        break;
    case IR_type_float:
        TYPE(iir)->id = TYPE_FLOAT;
        TYPE(iir)->data.ptr = NULL;
        TYPE(iir)->flags = 0;
        break;
    case IR_type_ptr:
        TYPE(iir)->id = TYPE_PTR;
        TYPE(iir)->data.ptr = NULL;
        TYPE(iir)->flags = 0;
        break;
    case IR_type_string:
        TYPE(iir)->id = TYPE_STRING;
        TYPE(iir)->data.ptr = NULL;
        TYPE(iir)->flags = 0;
        break;
    case IR_type_word:
        TYPE(iir)->id = TYPE_WORD;
        TYPE(iir)->data.ptr = NULL;
        TYPE(iir)->flags = 0;
        break;
    case IR_type_direct: {
        iIR x = SEMT(primary, 0);
        if (TYPE(x)->id != TYPE_STRUCT_DEF)
            throw("referenced non-struct id as type");

        TYPE(iir)->id = TYPE_STRUCT_INST;
        TYPE(iir)->data.word = x;
        TYPE(iir)->flags = 0;
        break;
    }
    case IR_type_tuple:
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
        TYPE(iir)->id = TYPE_BOOL;
        TYPE(iir)->data.ptr = NULL;
        TYPE(iir)->flags = 0;
        break;
    case IR_lit_word:
        TYPE(iir)->id = TYPE_WORD;
        TYPE(iir)->data.ptr = NULL;
        TYPE(iir)->flags = 0;
        break;
    case IR_lit_float:
        todo();
        break;
    case IR_lit_string:
        TYPE(iir)->id = TYPE_STRING;
        TYPE(iir)->data.ptr = NULL;
        TYPE(iir)->flags = 0;
        break;
    case IR_lit_tuple:
        todo();
        break;
    default:
        UNREACHABLE;
    }
}

static iIR sem_primary(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);
    iIR ret = 0;

    switch (type) {
    case IR_primary_dot:
        todo();
        break;
    case IR_primary_typed:
        todo();
        break;
    case IR_primary_id: {
        const char *name;
        name = get_id(ctx, GET_IRID(0));
        ret = lookup(ctx, name);
        assert(*buffer_get(ctx->mangling, ret, void *));
        buffer_set(ctx->mangling, iir, buffer_get(ctx->mangling, ret, void));
        *TYPE(iir) = *TYPE(ret);
        break;
    }
    default:
        UNREACHABLE;
    }

    return ret;
}

static void sem_expr(Ctx *ctx, iIR iir, IRType type);
static Types sem_expr_list(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);
    Types ret = NULL;

    switch (type) {
    case IR_expression_list_none:
        return ret;
    case IR_expression_list:
        break;
    default:
        UNREACHABLE;
    }

    iir = GET_IRID(0);
    type = GET_IRTYPE(0);
    ir = GET_IR(iir);
    buffer_new(&ret, sizeof(Type));

    for (;;) {
        switch (type) {
        case IR_expression_list_one:
            SEMT(expr, 0);
            buffer_push(ret, TYPE_CHILD(0));
            return ret;
        case IR_expression_list_rec:
            SEMT(expr, 0);
            buffer_push(ret, TYPE_CHILD(0));

            iir = GET_IRID(2);
            type = GET_IRTYPE(2);
            ir = GET_IR(iir);
            break;
        default:
            UNREACHABLE;
        }
    }

    return ret;
}

static void sem_call(Ctx *ctx, iIR iir) {
    IR *ir = GET_IR(iir);
    iIR ifunc;
    Function *func;
    Types expected, given;
    size_t i;

    ifunc = SEMT(primary, 0);
    assert(TYPE(ifunc)->id == TYPE_FUNC);
    func = (Function *)(TYPE(ifunc)->data.ptr);
    expected = func->params;
    given = SEMT(expr_list, 2);

    *TYPE(iir) = func->ret;

    if (!expected && !given) {
        return; /* ok */
    } else if (!expected && given) {
        throw("function did not expect parameters");
    } else if (expected && !given) {
        throw("function expected parameters");
    } else if (buffer_num(expected) != buffer_num(given)) {
        throw("invalid number of arguments for function");
    }

    for (i = 0; i < buffer_num(expected); ++i) {
        Type *a = buffer_get(expected, i, Type);
        Type *b = buffer_get(given, i, Type);
        if (!check_types(*a, *b))
            throwe("unexpected type for argument %lu", i);
    }
}

static void sem_struct_inst(Ctx *ctx, iIR iir) {
    IR *ir = GET_IR(iir);
    IRType irtype;
    iIR base;
    map fields;
    set seen = NULL;

    base = SEMT(primary, 0);
    if (TYPE(base)->id != TYPE_STRUCT_DEF)
        throw("cannot instantiate non-struct");

    TYPE(iir)->id = TYPE_STRUCT_INST;
    TYPE(iir)->data.word = base;
    TYPE(iir)->flags = 0;

    /* Go through the fields */
    fields = (map)(TYPE(base)->data.ptr);
    set_new_string(&seen);
    iir = GET_IRID(2);
    irtype = GET_IRTYPE(2);
    for (;;) {
        ir = GET_IR(iir);

        switch (irtype) {
        case IR_struct_field_inst_rec: {
            const char *field_name;
            Type *expected_type;
            assert(GET_IRTYPE(0) == IR_TOKEN);
            field_name = get_id(ctx, GET_IRID(0));

            /* Already seen? */
            if (set_has(seen, field_name))
                throwe("repeated field: %s", field_name);

            /* Check field exists */
            if (!map_has(fields, field_name))
                throwe("undefined field: %s", field_name);

            /* Compare types */
            expected_type = map_get(fields, field_name, Type);
            SEMT(expr, 2);
            if (!check_types(*expected_type, *TYPE_CHILD(2)))
                throwe("invalid type for field: %s", field_name);

            /* Mark field as seen */
            set_add(seen, field_name);

            /* Next one */
            iir = GET_IRID(4);
            irtype = GET_IRTYPE(4);
            break;
        }
        case IR_struct_field_inst_null:
            if (map_num(fields) != set_num(seen))
                throw("struct initialization with missing fields");
            return;
        default:
            UNREACHABLE;
        }
    }
}

static void sem_assign(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);
    iIR lhs;

    /* Check lhs is a valid lvalue */
    lhs = SEMT(primary, 0);
    if (!(TYPE(lhs)->flags & TYPE_FLAG_MUTABLE))
        throw("assign to immutable variable");

    /* Check type of rhs matches */
    SEMT(expr, 2);
    if (!check_types(*TYPE(lhs), *TYPE_CHILD(2)))
        throw("type mismatch in assignment");

    /* Check assignment can happen */
    (void)type; /* TODO */
    /*switch (type) {
    case IR_assign_eq:
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
        }*/
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
        sem_call(ctx, iir);
        break;
    case IR_expr_lit:
        SEMT(lit, 0);
        COPY_TYPE(0);
        break;
    case IR_expr_sizeof:
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
        todo();
        break;
    case IR_expr_struct_inst:
        SEM(struct_inst, 0);
        COPY_TYPE(0);
        break;
    case IR_expr_assign:
        SEMT(assign, 0);
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
        assert(GET_IRTYPE(2) == IR_TOKEN);
        name = get_id(ctx, GET_IRID(2));
        mangled = mangle_var(ctx, name);
        buffer_set(ctx->mangling, iir, &mangled);
        SEMT(expr, 4);
        COPY_TYPE(4);
        TYPE(iir)->flags |= TYPE_FLAG_MUTABLE;
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
        SEMT(expr, 0);
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
static Types sem_func_params(Ctx *ctx, iIR iir, IRType type) {
    Types ret = NULL;
    Type aux;

    buffer_new(&ret, sizeof(Type));

    for (;;) {
        IR *ir = GET_IR(iir);
        switch (type) {
        case IR_params_rec:
            SEMT(func_param, 0);
            aux = *TYPE_CHILD(0);
            buffer_push(ret, &aux);

            iir = GET_IRID(1);
            type = GET_IRTYPE(1);
            break;
        case IR_params_one:
            SEMT(func_param, 0);
            aux = *TYPE_CHILD(0);
            buffer_push(ret, &aux);
            return ret;
        default:
            UNREACHABLE;
        }
    }
}

static void sem_func(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);
    const char *name;
    Function *func;
    char *mangled;
    size_t block;

    /* TODO: annotations */

    func = malloc(sizeof(Function));
    assert(GET_IRTYPE(2) == IR_TOKEN);
    name = get_id(ctx, GET_IRID(2));
    push_to_scope(ctx, name, iir);
    push_scope(ctx);

    switch (type) {
    case IR_function_noargs_void:
        func->params = NULL;
        func->ret.id = TYPE_NOTHING;
        func->ret.data.ptr = NULL;
        func->ret.flags = 0;
        block = 3;
        break;
    case IR_function_noargs_typed:
        SEMT(type, 4);
        func->params = NULL;
        func->ret = *TYPE_CHILD(4);
        block = 5;
        break;
    case IR_function_void:
        func->params = SEMT(func_params, 4);
        func->ret.id = TYPE_NOTHING;
        func->ret.data.ptr = NULL;
        func->ret.flags = 0;
        block = 6;
        break;
    case IR_function_typed:
        SEMT(type, 7);
        func->params = SEMT(func_params, 4);
        func->ret = *TYPE_CHILD(7);
        block = 8;
        break;
    default:
        UNREACHABLE;
    }

    /* Save type information */
    TYPE(iir)->id = TYPE_FUNC;
    TYPE(iir)->data.ptr = func;
    TYPE(iir)->flags = 0;
    buffer_push(ctx->funcrets, &func->ret);

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

static void sem_struct(Ctx *ctx, iIR iir) {
    iIR struct_def = iir;
    IR *ir = GET_IR(iir);
    const char *name;
    char *mangled;
    IRType type;
    map fields = NULL; /* map<string, Type> */

    /* TODO: annotations */

    assert(GET_IRTYPE(2) == IR_TOKEN);
    name = get_id(ctx, GET_IRID(2));
    if (in_scope(ctx, name))
        throwe("already declared: %s", name);
    mangled = mangle(ctx, name);
    buffer_set(ctx->mangling, iir, &mangled);

    /* Run through the fields */
    map_new_string(&fields, sizeof(Type), NULL, NULL, NULL, NULL);
    iir = GET_IRID(4);
    type = GET_IRTYPE(4);
    for (;;) {
        ir = GET_IR(iir);
        switch (type) {
        case IR_struct_def: {
            /* TYPED_ID */
            const char *field_name;
            Type field_type;

            field_name = SEM(typed_id, 0);
            field_type = *TYPE(GET_IRID(0));
            map_add(fields, field_name, &field_type);

            /* Recursion */
            iir = GET_IRID(2);
            type = GET_IRTYPE(2);
            break;
        }
        case IR_struct_def_null:
            goto done;
        default:
            UNREACHABLE;
        }
    }

done:
    TYPE(struct_def)->id = TYPE_STRUCT_DEF;
    TYPE(struct_def)->data.ptr = fields;
    TYPE(struct_def)->flags = 0;
    push_to_scope(ctx, name, struct_def);
}

static Types sem_extern_params(Ctx *ctx, iIR iir, IRType type) {
    /* Recycle sem_func_params, check for invalid types */
    Types ret = sem_func_params(ctx, iir, type);
    size_t i;

    for (i = 0; i < buffer_num(ret); ++i) {
        switch (buffer_get(ret, i, Type)->id) {
        case TYPE_BYTE:
        case TYPE_FLOAT:
        case TYPE_PTR:
        case TYPE_WORD:
            /* Valid types */
            break;
        default:
            throw("invalid type for extern parameter");
        }
    }

    return ret;
}

static void sem_extern(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);
    const char *name;
    Function *func;

    func = malloc(sizeof(Function));
    assert(GET_IRTYPE(2) == IR_TOKEN);
    name = get_id(ctx, GET_IRID(2));
    push_to_scope(ctx, name, iir);

    switch (type) {
    case IR_extern_noargs_void:
        todo();
        break;
    case IR_extern_noargs_typed:
        todo();
        break;
    case IR_extern_void:
        func->params = SEMT(extern_params, 4);
        func->ret.id = TYPE_NOTHING;
        func->ret.data.ptr = NULL;
        func->ret.flags = 0;
        break;
    case IR_extern_typed:
        todo();
        break;
    default:
        UNREACHABLE;
    }

    /* Save type information */
    TYPE(iir)->id = TYPE_FUNC;
    TYPE(iir)->data.ptr = func;
    TYPE(iir)->flags = 0;

    buffer_set(ctx->mangling, iir, &name);
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
        SEM(struct, 0);
        break;
    case IR_global_impl:
        todo();
        break;
    case IR_global_extern:
        SEMT(extern, 0);
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
