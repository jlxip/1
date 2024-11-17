#include <common.h>
#include <compiler.h>
#include <error.h>
#include <stdlib.h>
#include <tokens.h>

/* An uncomplicated recursive descent parser */

/* Array of captures */
static Capture *v;
static size_t vlen;
/* Current capture */
static Capture *c;
static size_t cpos;
/* Extra */
static size_t any_errors;

void error(const char *msg) {
    char buf[1024] = {0};
    sprintf(buf, "syntax error: %s", msg);
    log_error(buf, 0, c->lineno);
    any_errors = 1;
}

#define expected(X)                                                            \
    do {                                                                       \
        if (required)                                                          \
            error("expected" X);                                               \
        return 0;                                                              \
    } while (0)

#define ACCEPT return 1

static size_t next(void) {
    if (cpos == vlen - 1) {
        error("reached end of input file");
        return 0;
    }

    ++c;
    ++cpos;
    return 1;
}

/*static size_t maybe_next(void) {
    if (cpos == vlen - 1)
        return 0;
    ++c;
    ++cpos;
    return 1;
}*/

/*static void back(void) {
    assert(cpos);
    --c;
    --cpos;
}*/

#define accept(X) (c->token == (X))

/*
    TODO:
    - AND: logic
    - AS: import, with
    - BREAK: statement
    - CLASS: statement
    - CONTINUE: statement
    - DEF: statement
    - DEL: statement
    - ELIF: if
    - ELSE: if
    - FOR: statement
    - FROM: import
    - GLOBAL: statement
    - IF: statement
    - IMPORT: import
    - IN: logic, loop
    - IS: logic
    - LAMBDA: expression
    - NOT: logic
    - OR: logic
    - PASS: expresion
    - RETURN: expression
    - WHILE: expression
    - WITH: expression

    - NONE
    - BOOL
    - INT
    - FLOAT
    - STRING

    - DEQ: comp
    - NEQ: comp
    - EQ: assign
    - PLUSEQ: assign
    - PLUS: arith
    - MINUSEQ: assign
    - MINUS: arith
    - STAREQ: assign
    - STAR: arith
    - DSLASHEQ: assign
    - DSLASH: arith
    - SLASHEQ: assign
    - SLASH: arith
    - HATEQ: assig
    - HAT: bit
    - AMPEQ: assign
    - AMP: bit
    - LEQ: comp
    - LT: comp
    - GEQ: comp
    - GT: comp
    - DOT
    - COMMA
    - COLON
    - SEMICOLON
    - ATSYMBOL
    - OPAR: terminal expresion, def, call, tuple
    - CPAR: terminal expresion, def, call, tuple
    - OBRACKET: list
    - CBRACKET: list
    - OBRACES: set, dict
    - CBRACES: set, dict
    - ID: expression
*/

/* ---------------------------------------- */

/*static size_t terminal_expression(size_t);
static size_t logical_expression(size_t);
static size_t expression(size_t);*/
static size_t statement(size_t);

/* ---------------------------------------- */

#define ANYLITERAL                                                             \
    case T_NONE:                                                               \
    case T_BOOL:                                                               \
    case T_INT:                                                                \
    case T_FLOAT:                                                              \
    case T_STRING

/*static size_t call(size_t required) {  }*/

/*static size_t terminal_expression(size_t required) {
     Certain expressions, left asserted: "1", "x", "(1+1)", "call()" 
    if (call(0))
      todo();

     id, call, parentheses, literals
    switch (c->token) {
    case T_ID:
         "id" or "id()"?
        if(maybe_next()) {
            if (c.token ==
        }
        ACCEPT;
    ANYLITERAL:
        ACCEPT;
    default:
        return required;
    }
}*/

/*static size_t logical_expression(size_t required) {
     Formed by logic operations on expressions
    (void)required;
}*/

/*static size_t expression(size_t required) {
     An expression is evaluable: "not 1", "2 + 2", "call()"
}*/

static size_t statement(size_t required) {
    /*if (expression(0))
      goto ok;*/

    todo();

    /*ok:*/
    if (!next())
        goto nosemi;
    else if (!accept(T_SEMICOLON))
        goto nosemi;
    ACCEPT;

nosemi:
    expected("semicolon at the end of statement");
}

void parse(const char *code) {
    buffer buf = get_tokens(code);
    v = (Capture *)buf->a;
    vlen = buf->used;

    c = v;
    cpos = 0;
    any_errors = 0;

    if (!next()) {
        error("nothing to parse!");
        goto end;
    }

    statement(1);

end:
    free(v);
}
