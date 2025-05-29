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

Type _NULL_TYPE() {
    Type ret;
    ret.id = TYPE_NOTHING;
    ret.data.ptr = NULL;
    ret.flags = 0;
    return ret;
}
#define NULL_TYPE _NULL_TYPE()

#define IIR_BEGIN (buffer_num(ctx.irs) - 3)
#define IIR_CONST_SELF (buffer_num(ctx->irs) - 2)
#define IIR_MUT_SELF (buffer_num(ctx->irs) - 1)

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

static const char *get_id(Ctx *ctx, iToken itoken) {
    Token *token = GET_TOKEN(itoken);
    assert(token->id == T_ID);
    return token->data.str;
}

static bool check_types(Type a, Type b) {
    if (!a.id)
        throw("TYPE_UNKNOWN! (bug, a)");
    if (!b.id)
        throw("TYPE_UNKNOWN! (bug, b)");

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

static void sem_expr(Ctx *ctx, iIR iir, IRType type);
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
    case IR_type_expr: {
        SEMT(expr, 0);
        if (TYPE_CHILD(0)->id != TYPE_STRUCT_DEF)
            throw("referenced non-struct id as type");
        COPY_TYPE(0);
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
        TYPE(iir)->id = TYPE_FLOAT;
        TYPE(iir)->data.ptr = NULL;
        TYPE(iir)->flags = 0;
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

static void sem_expr(Ctx *ctx, iIR iir, IRType type);
static Types sem_expr_list(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);
    Types ret = NULL;
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

static void parse_annotation(Ctx *ctx, iIR iir, IRType type, map out) {
    IR *ir = GET_IR(iir);
    const char *name;
    Types types;

    assert(type == IR_annot || type == IR_annot_args);
    assert(GET_IRTYPE(1) == IR_TOKEN);
    name = get_id(ctx, GET_IRID(1));
    if (map_has(out, name))
        throwe("@%s already used", name);

    switch (type) {
    case IR_annot:
        types = NULL;
        break;
    case IR_annot_args:
        types = SEMT(expr_list, 3);
        break;
    default:
        UNREACHABLE;
    }

    map_add(out, name, &types);
}

static map sem_annotations(Ctx *ctx, iIR iir, IRType type) {
    map ret = NULL; /* map<const char*, Types> */
    map_new_string(&ret, sizeof(Types), NULL, NULL, NULL, NULL);

    for (;;) {
        IR *ir = GET_IR(iir);
        switch (type) {
        case IR_annot_rec:
            parse_annotation(ctx, GET_IRID(0), GET_IRTYPE(0), ret);
            iir = GET_IRID(1);
            type = GET_IRTYPE(1);
            break;
        case IR_annot_null:
            return ret;
        default:
            UNREACHABLE;
        }
    }
}

static void sem_call(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);
    Function *func;
    Types expected, given;
    size_t i;

    SEMT(expr, 0);
    assert(TYPE(GET_IRID(0))->id == TYPE_FUNC);
    func = (Function *)(TYPE(GET_IRID(0))->data.ptr);
    expected = func->params;

    switch (type) {
    case IR_expr_call_noargs:
        given = NULL;
        break;
    case IR_expr_call:
        given = SEMT(expr_list, 2);
        break;
    default:
        UNREACHABLE;
    }

    assert(func->ret.id);
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
    Struct *obj;
    set seen = NULL;

    SEMT(expr, 0);
    if (TYPE_CHILD(0)->id != TYPE_STRUCT_DEF)
        throw("cannot instantiate non-struct");
    obj = (Struct *)(TYPE_CHILD(0)->data.ptr);
    TYPE(iir)->id = TYPE_STRUCT_INST;
    TYPE(iir)->data.word = obj->this;
    TYPE(iir)->flags = 0;

    /* Go through the fields */
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
            if (!map_has(obj->fields, field_name))
                throwe("undefined field: %s", field_name);

            /* Compare types */
            expected_type = map_get(obj->fields, field_name, Type);
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
            if (map_num(obj->fields) != set_num(seen))
                throw("struct initialization with missing fields");
            return;
        default:
            UNREACHABLE;
        }
    }
}

static void sem_assign(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);

    /* Check lhs is a valid lvalue */
    SEMT(expr, 0);
    if (!IS_MUTABLE(GET_IRID(0)))
        throw("assign to immutable variable");

    /* Check type of rhs matches */
    SEMT(expr, 2);
    if (!check_types(*TYPE_CHILD(0), *TYPE_CHILD(2)))
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
        SEMT(expr, 1);
        COPY_TYPE(1);
        break;
    case IR_expr_id: {
        iIR ref;
        assert(GET_IRTYPE(0) == IR_TOKEN);
        ref = lookup(ctx, get_id(ctx, GET_IRID(0)));
        assert(*buffer_get(ctx->mangling, ref, void *));
        buffer_set(ctx->mangling, iir, buffer_get(ctx->mangling, ref, void));
        *TYPE(iir) = *TYPE(ref);
        break;
    }
    case IR_expr_dot: {
        const char *name;
        SEMT(expr, 0);
        assert(GET_IRTYPE(2) == IR_TOKEN);
        name = get_id(ctx, GET_IRID(2));

        switch (TYPE_CHILD(0)->id) {
        case TYPE_STRUCT_DEF: {
            Struct *obj = (Struct *)(TYPE_CHILD(0)->data.ptr);
            if (map_has(obj->fields, name)) {
                todo();
            } else if (map_has(obj->methods, name)) {
                iIR method = *map_get(obj->methods, name, iIR);
                if (!IS_STATIC(method))
                    throwe("method `%s' is non-static", name);
                *TYPE(iir) = *TYPE(method);
            } else
                throwe("no `%s' in struct", name);
            break;
        }
        case TYPE_STRUCT_INST: {
            iIR struct_def = TYPE_CHILD(0)->data.word;
            Struct *obj = (Struct *)(TYPE(struct_def)->data.ptr);
            if (map_has(obj->fields, name)) {
                *TYPE(iir) = *map_get(obj->fields, name, Type);
            } else if (map_has(obj->methods, name)) {
                iIR method = *map_get(obj->methods, name, iIR);
                if (IS_STATIC(method))
                    throwe("method `%s' is static, use struct", name);
                *TYPE(iir) = *TYPE(method);
            } else
                throwe("no `%s' in self", name);
            break;
        }
        case TYPE_SELF: {
            iIR struct_def;
            Struct *obj;
            struct_def = lookup(ctx, "Self");
            obj = (Struct *)(TYPE(struct_def)->data.ptr);

            if (map_has(obj->fields, name)) {
                *TYPE(iir) = *map_get(obj->fields, name, Type);
            } else if (map_has(obj->methods, name)) {
                iIR method = *map_get(obj->methods, name, iIR);
                if (IS_STATIC(method))
                    throwe("method `%s' is static, use Self", name);
                *TYPE(iir) = *TYPE(method);
            } else
                throwe("no `%s' in self", name);
            break;
        }
        default:
            UNREACHABLE;
        }
        break;
    }
    case IR_expr_call_noargs:
        sem_call(ctx, iir, type);
        break;
    case IR_expr_call:
        sem_call(ctx, iir, type);
        break;
    case IR_expr_lit:
        SEMT(lit, 0);
        COPY_TYPE(0);
        break;
    case IR_expr_sizeof:
    case IR_expr_hat:
    case IR_expr_amp:
    case IR_expr_bar:
        todo();
        break;
    case IR_expr_star:
        SEMT(expr, 0);
        SEMT(expr, 2);
        switch (TYPE_CHILD(0)->id) {
        case TYPE_WORD:
            if (TYPE_CHILD(2)->id != TYPE_WORD)
                throw("tried to multiply word and non-word");
            COPY_TYPE(0);
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
        SEM(struct_inst, 0);
        COPY_TYPE(0);
        break;
    case IR_expr_assign:
        SEMT(assign, 0);
        break;
    default:
        UNREACHABLE;
    }

    if (!TYPE(iir)->id)
        throwe("Oops this is a bug. Expression type: %u", type);
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
    default:
        UNREACHABLE;
    }

    push_to_scope(ctx, name, iir);
}

static void sem_block(Ctx *ctx, iIR iir);
static void sem_stmt(Ctx *ctx, iIR iir, IRType type) {
    IR *ir = GET_IR(iir);

    switch (type) {
    case IR_stmt_block:
        SEM(block, 0);
        break;
    case IR_stmt_decl:
        SEMT(decl_nonglobal, 0);
        break;
    case IR_stmt_expr:
        SEMT(expr, 0);
        break;
    case IR_stmt_assert:
        SEMT(expr, 1);
        switch (TYPE_CHILD(1)->id) {
        case TYPE_BOOL:
        case TYPE_BYTE:
        case TYPE_FLOAT:
        case TYPE_PTR:
        case TYPE_STRING:
        case TYPE_WORD:
            /* These are OK */
            break;
        case TYPE_TUPLE:
            throw("bool downcast is undefined for tuples");
            break;
        case TYPE_STRUCT_INST:
            todo();
            break;
        default:
            throw("bool downcast does not apply here");
        }
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
    map annot; /* map<const char*, Types> */
    Function *func;
    const char *name;
    size_t flags = 0;
    size_t block;
    bool colon_mode = false;
    char *mangled;

    /* Annotations */
    annot = SEMT(annotations, 0);
    if (map_has(annot, "static")) {
        if (!in_scope(ctx, "Self"))
            throw("@static used in non-method");
        flags |= TYPE_FLAG_STATIC;
        map_remove(annot, "static");
    }

    if (!map_empty(annot))
        throw("unused annotations");

    func = malloc(sizeof(Function));
    assert(GET_IRTYPE(2) == IR_TOKEN);
    name = get_id(ctx, GET_IRID(2));
    push_to_scope(ctx, name, iir);
    push_scope(ctx);

    /* Push self */
    if (in_scope(ctx, "Self")) {
        /* This is a method! */
        /* TODO: this does not happen in static methods */
        /* TODO: maybe mutable */
        push_to_scope(ctx, "self", IIR_CONST_SELF);
        flags |= TYPE_FLAG_METHOD;
    }

    switch (type) {
    case IR_function_noargs_void:
        func->params = NULL;
        func->ret = NULL_TYPE;
        block = 3;
        break;
    case IR_function_noargs_void_colon:
        func->params = NULL;
        func->ret = NULL_TYPE;
        block = 4;
        colon_mode = true;
        break;
    case IR_function_noargs_typed:
        SEMT(type, 4);
        func->params = NULL;
        func->ret = *TYPE_CHILD(4);
        block = 5;
        break;
    case IR_function_noargs_typed_colon:
        SEMT(type, 4);
        func->params = NULL;
        func->ret = *TYPE_CHILD(4);
        block = 6;
        colon_mode = true;
        break;
    case IR_function_void:
        func->params = SEMT(func_params, 4);
        func->ret = NULL_TYPE;
        block = 6;
        break;
    case IR_function_void_colon:
        func->params = SEMT(func_params, 4);
        func->ret = NULL_TYPE;
        block = 7;
        colon_mode = true;
        break;
    case IR_function_typed:
        SEMT(type, 7);
        func->params = SEMT(func_params, 4);
        func->ret = *TYPE_CHILD(7);
        block = 8;
        break;
    case IR_function_typed_colon:
        SEMT(type, 7);
        func->params = SEMT(func_params, 4);
        func->ret = *TYPE_CHILD(7);
        block = 9;
        colon_mode = true;
        break;
    default:
        UNREACHABLE;
    }

    if (!func->ret.id)
        throwe("yankee with no brim? Function type: %u\n", type);

    if (func->ret.id == TYPE_STRUCT_DEF) {
        /* If a struct is returned, it's actually an instance */
        Struct *obj = (Struct *)(func->ret.data.ptr);
        func->ret.id = TYPE_STRUCT_INST;
        func->ret.data.word = obj->this;
        func->ret.flags = 0;
    }

    /* Save type information */
    TYPE(iir)->id = TYPE_FUNC;
    TYPE(iir)->data.ptr = func;
    TYPE(iir)->flags = flags;
    buffer_push(ctx->funcrets, &func->ret);

    buffer_set(ctx->rawnames, iir, &name);
    mangled = mangle(ctx, name);
    buffer_set(ctx->mangling, iir, &mangled);

    if (colon_mode)
        SEMT(expr, block);
    else
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
        assert(GET_IRTYPE(2) == IR_TOKEN);
        name = get_id(ctx, GET_IRID(2));
        if (in_scope(ctx, name))
            throwe("already declared: %s", name);
        mangled = mangle(ctx, name);
        buffer_set(ctx->mangling, iir, &mangled);
        SEMT(expr, 4);
        COPY_TYPE(4);
        TYPE(iir)->flags |= TYPE_FLAG_MUTABLE;
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
    Struct *obj = calloc(1, sizeof(Struct));

    /* TODO: annotations */

    assert(GET_IRTYPE(2) == IR_TOKEN);
    name = get_id(ctx, GET_IRID(2));
    if (in_scope(ctx, name))
        throwe("already declared: %s", name);
    buffer_set(ctx->rawnames, iir, &name);
    mangled = mangle(ctx, name);
    buffer_set(ctx->mangling, iir, &mangled);

    /* Run through the fields */
    map_new_string(&obj->fields, sizeof(Type), NULL, NULL, NULL, NULL);
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
            map_add(obj->fields, field_name, &field_type);

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
    map_new_string(&obj->methods, sizeof(iIR), NULL, NULL, NULL, NULL);
    obj->this = struct_def;
    TYPE(struct_def)->id = TYPE_STRUCT_DEF;
    TYPE(struct_def)->data.ptr = obj;
    TYPE(struct_def)->flags = 0;
    push_to_scope(ctx, name, struct_def);
}

static void sem_impl(Ctx *ctx, iIR iir) {
    IR *ir = GET_IR(iir);
    iIR struct_def;
    Struct *obj;
    const char *struct_name;
    IRType type;
    const char *method_name;

    SEMT(expr, 2);
    if (TYPE_CHILD(2)->id != TYPE_STRUCT_DEF)
        throw("tried to impl a non-struct");
    obj = (Struct *)(TYPE_CHILD(2)->data.ptr);
    struct_def = obj->this;

    /* This comes very handy for emit */
    TYPE(iir)->data.word = struct_def;

    /* Add Self */
    push_scope(ctx);
    push_to_scope(ctx, "Self", struct_def);

    /* Add prefix for mangling */
    struct_name = *buffer_get(ctx->rawnames, struct_def, const char *);
    assert(struct_name);
    buffer_push(ctx->mangling_stack, &struct_name);

    iir = GET_IRID(4);
    type = GET_IRTYPE(4);
    for (;;) {
        ir = GET_IR(iir);
        switch (type) {
        case IR_impl_def:
            SEMT(func, 0);
            method_name = *buffer_get(ctx->rawnames, GET_IRID(0), const char *);
            map_add(obj->methods, method_name, &GET_IRID(0));
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
    pop_scope(ctx);
    buffer_pop(ctx->mangling_stack);
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
        func->params = NULL;
        func->ret.id = TYPE_NOTHING;
        func->ret.data.ptr = NULL;
        func->ret.flags = 0;
        break;
    case IR_extern_noargs_typed:
        func->params = NULL;
        func->ret = *TYPE_CHILD(4);
        break;
    case IR_extern_void:
        func->params = SEMT(extern_params, 4);
        func->ret.id = TYPE_NOTHING;
        func->ret.data.ptr = NULL;
        func->ret.flags = 0;
        break;
    case IR_extern_typed:
        func->params = SEMT(extern_params, 4);
        func->ret = *TYPE_CHILD(7);
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
        SEM(impl, 0);
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

static void add_self(Ctx *ctx) {
    IR ir;
    Type type;
    const char *txt = "self";

    ir.types = NULL;
    ir.ids = NULL;
    buffer_push(ctx->irs, &ir);
    type.id = TYPE_SELF;
    type.data.ptr = NULL;
    type.flags = 0;
    buffer_push(ctx->types, &type);
    buffer_push(ctx->mangling, &txt);

    buffer_push(ctx->irs, &ir);
    type.flags = TYPE_FLAG_MUTABLE;
    buffer_push(ctx->types, &type);
    buffer_push(ctx->mangling, &txt);
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

    /* Add context-free "self" */
    add_self(&ctx);

    /* State: raw names */
    ctx.rawnames = NULL;
    buffer_new(&ctx.rawnames, sizeof(const char *));
    buffer_zresize(ctx.rawnames, buffer_num(ctx.irs));
    /* State: symbol tables */
    ctx.tables = NULL;
    buffer_new(&ctx.tables, sizeof(SymbolTable));
    buffer_zresize(ctx.tables, buffer_num(ctx.irs));
    /* State: mangling stack */
    ctx.mangling_stack = NULL;
    buffer_new(&ctx.mangling_stack, sizeof(const char *));
    /* State: expected return type of called functions */
    ctx.funcrets = NULL;
    buffer_new(&ctx.funcrets, sizeof(Type));
    /* State: shadowing stack */
    ctx.shadowing = NULL;
    buffer_new(&ctx.shadowing, sizeof(ShadowingTable));

    sem_program(&ctx, IIR_BEGIN);

    ret.types = ctx.types;
    ret.mangling = ctx.mangling;
    return ret;
}
