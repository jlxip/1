#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"

#ifdef DEBUG_PRINT_TOKENS
#define CREATE_TOKEN_STRINGS
#endif

#include <common.h>
#include <compiler.h>
#include <string.h>
#include <tokens.h>

/* Global state */
static size_t lineno = 0;

#define OK_TOKEN(X)                                                            \
    do {                                                                       \
        ret.ok = 1;                                                            \
        ret.consumed = cur - begin;                                            \
        ret.lineno = lineno;                                                   \
        ret.token = X;                                                         \
        return ret;                                                            \
    } while (0)
#define OK_TOKEN_INFO(X, Y)                                                    \
    do {                                                                       \
        ret.ok = 1;                                                            \
        ret.consumed = cur - begin;                                            \
        ret.lineno = lineno;                                                   \
        ret.token = X;                                                         \
        ret.info = Y;                                                          \
        return ret;                                                            \
    } while (0)

#define END_TOKEN                                                              \
    case '\n':                                                                 \
        ++lineno;                                                              \
    case ' ':                                                                  \
    case '\0'

#define CASE_NUMBERS                                                           \
    case '0':                                                                  \
    case '1':                                                                  \
    case '2':                                                                  \
    case '3':                                                                  \
    case '4':                                                                  \
    case '5':                                                                  \
    case '6':                                                                  \
    case '7':                                                                  \
    case '8':                                                                  \
    case '9'

/* Check for END_TOKEN, then raise OK_TOKEN */
#define MAYBE_TOKEN(X)                                                         \
    do {                                                                       \
        switch (*cur++) {                                                      \
        END_TOKEN:                                                             \
            OK_TOKEN(X);                                                       \
        }                                                                      \
    } while (0)

/* Check the rest of the token, in case there's only one match. Keywords only */
static size_t _match(const char **cur, const char *target) {
    for (;;) {
        char a = *(*cur)++;
        char b = *target++;

        /* End of target = probably match */
        if (!b) {
            if (!((a >= 'A' && a <= 'Z') || (a >= 'a' && a <= 'z'))) {
                --(*cur); /* go back */
                return 1;
            }
            switch (a) {
            END_TOKEN:
                --(*cur); /* go back */
                return 1;
            default:
                return 0;
            }
        }

        /* End of cursor = mismatch */
        if (!a)
            return 0;

        if (a != b)
            return 0;
    }
}

/* Call _match, maybe call OK_TOKEN */
#define MATCH_REST(P, T)                                                       \
    if (_match(&cur, P))                                                       \
        OK_TOKEN(T);
#define MATCH_REST_INFO(P, T, I)                                               \
    if (_match(&cur, P))                                                       \
        OK_TOKEN_INFO(T, I);

/* Regex-like matches for identifiers */
#define IS_AZ(X) ((X) >= 'A' && (X) <= 'Z')
#define IS_az(X) ((X) >= 'a' && (X) <= 'z')
#define IS_09(X) ((X) >= '0' && (X) <= '9')
#define IS__(X) ((X) == '_')

/* ID */
static size_t handle_id(Capture *ret, const char **cur) {
    const char *begin = *cur;
    char *buf = NULL; /* Copied in case of success */
    size_t len;

    /* First character must be [A-Za-z_] */
    char c = *(*cur)++;
    if (!(IS_AZ(c) || IS_az(c) || IS__(c)))
        return 0;

    /* Next characters are in [A-Za-z0-9_] */
    for (len = 1;; ++len) {
        char c = *(*cur)++;
        switch (c) {
        END_TOKEN:
            buf = malloc(len + 1);
            memcpy(buf, begin, len);
            buf[len] = '\0';
            ret->ok = 1;
            ret->consumed = len;
            ret->token = T_ID;
            ret->info = (size_t)buf;
            return 1;
        }

        if (!(IS_AZ(c) || IS_az(c) || IS_09(c) || IS__(c)))
            return 0;
    }
}

/* Numeric literals */
static size_t handle_numeric(Capture *ret, const char **cur) {
    const char *begin = *cur;
    char *buf = NULL;   /* In case of int */
    double *val = NULL; /* In case of float */
    size_t consumed = 0;

    /* State 0: only numbers */
    for (;;) {
        char c = *(*cur)++;
        switch (c) {
        CASE_NUMBERS:
            ++consumed;
            break;
        case '.':
            ++consumed;
            goto state1;
        case 'e':
            ++consumed;
            goto state2;
        default:
            /* It's an int! */
            buf = malloc(consumed + 1);
            memcpy(buf, begin, consumed);
            buf[consumed] = '\0';
            ret->ok = 1;
            ret->consumed = consumed;
            ret->token = T_INT;
            ret->info = (size_t)buf;
            return 1;
        }
    }

state1:
    /* State 1: a dot has appeared, must be float, but no more dots! */
    for (;;) {
        char c = *(*cur)++;
        ++consumed;
        switch (c) {
        CASE_NUMBERS:
            ++consumed;
            break;
        case 'e':
            ++consumed;
            goto state2;
        default:
            /* It's a float! */
            val = malloc(sizeof(double));
            *val = atof(begin);
            ret->ok = 1;
            ret->consumed = consumed;
            ret->token = T_FLOAT;
            ret->info = (size_t)val;
            return 1;
        }
    }

state2:
    /* State 2: an 'e' has appeared, must be float, but no more symbols! */
    for (;;) {
        char c = *(*cur)++;
        switch (c) {
        CASE_NUMBERS:
            ++consumed;
            break;
        default:
            /* It's a float! */
            val = malloc(sizeof(double));
            *val = atof(begin);
            ret->ok = 1;
            ret->consumed = consumed - 1;
            ret->token = T_FLOAT;
            ret->info = (size_t)val;
            return 1;
        }
    }
}

static size_t handle_string(Capture *ret, const char **cur) {
    /* The first quote has already been consumed */
    const char *begin = *cur;
    char *buf = NULL;
    size_t len;

    for (len = 0;; ++len) {
        char c = *(*cur)++;
        switch (c) {
        case '\\':
            /* Escape */
            todo(); /* not feeling like it */
        case '"':
            /* End */
            buf = malloc(len + 1);
            memcpy(buf, begin, len);
            buf[len] = '\0';
            ret->ok = 1;
            ret->consumed = len + 2;
            ret->token = T_STRING;
            ret->info = (size_t)buf;
            return 1;
        END_TOKEN:
            /* Unfinished string */
            todo();
            return 0;
        }
    }
}

/* Next token function */
static Capture next_token(const char *cur) {
    Capture ret;
    const char *begin = cur;
    ret.lineno = lineno;
    ret.info = 0;

    if (!*cur) {
        ret.ok = 2; /* end */
        ret.token = T_NULL;
        ret.consumed = 0;
        return ret;
    }

    /* --- KEYWORDS --- */

    switch (*cur++) {
    case 'a':
        /* a: and, as */
        switch (*cur++) {
        case 'n':
            /* an: and */
            MATCH_REST("d", T_AND);
        case 's':
            /* as: as */
            switch (*cur++) {
            END_TOKEN:
                /* as */
                OK_TOKEN(T_AS);
            }
        }
    case 'b':
        /* b: break */
        MATCH_REST("reak", T_BREAK);
    case 'c':
        /* c: continue, class */
        switch (*cur++) {
        case 'o':
            /* co: continue */
            MATCH_REST("ntinue", T_CONTINUE);
        case 'l':
            /* cl: class */
            MATCH_REST("ass", T_CLASS);
        }
    case 'd':
        /* d: def, del */
        switch (*cur++) {
        case 'e':
            /* de: def, del */
            switch (*cur++) {
            case 'f':
                /* def: def */
                MAYBE_TOKEN(T_DEF);
            case 'l':
                /* del: del */
                MAYBE_TOKEN(T_DEL);
            }
        }
    case 'e':
        /* e: elif, else */
        switch (*cur++) {
        case 'l':
            /* el: elif, else */
            switch (*cur++) {
            case 'i':
                /* eli: elif */
                MATCH_REST("f", T_ELIF);
            case 's':
                /* els: else */
                MATCH_REST("e", T_ELSE);
            }
        }
    case 'F':
        /* F: False */
        MATCH_REST_INFO("alse", T_BOOL, 0);
    case 'f':
        /* f: for, from */
        switch (*cur++) {
        case 'o':
            /* FO: for */
            MATCH_REST("r", T_FOR);
        case 'r':
            /* FR: from */
            MATCH_REST("om", T_FROM);
        }
    case 'g':
        /* g: global */
        MATCH_REST("lobal", T_GLOBAL);
    case 'i':
        /* i: in, is, if, import */
        switch (*cur++) {
        case 'n':
            MAYBE_TOKEN(T_IN);
        case 's':
            MAYBE_TOKEN(T_IS);
        case 'f':
            MAYBE_TOKEN(T_IF);
        case 'm':
            MATCH_REST("port", T_IMPORT);
        }
    case 'l':
        /* l: lambda */
        MATCH_REST("ambda", T_LAMBDA);
    case 'N':
        /* N: None */
        MATCH_REST("one", T_NONE);
    case 'n':
        /* n: not */
        MATCH_REST("ot", T_NOT);
    case 'o':
        /* o: or */
        MATCH_REST("r", T_OR);
    case 'r':
        /* r: return */
        MATCH_REST("turn", T_RETURN);
    case 'T':
        /* T: True */
        MATCH_REST_INFO("rue", T_BOOL, 1);
    case 'w':
        /* w: while, with */
        switch (*cur++) {
        case 'h':
            /* wh: while */
            MATCH_REST("ile", T_WHILE);
        case 'i':
            /* wi: with */
            MATCH_REST("th", T_WITH);
        }
    END_TOKEN:
        /* Nothing to do here */
        ret.consumed = 1;
        OK_TOKEN(T_NULL);
    }

    /* --- ID --- */

    cur = begin;
    if (handle_id(&ret, &cur))
        return ret;

    /* --- SYMBOLS --- */

    cur = begin;
    switch (*cur++) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        /* Numeric literal */
        cur = begin; /* Reset to grab the first number too */
        if (handle_numeric(&ret, &cur))
            return ret;
        /* TODO: lexical error */
        break;
    case '"':
        /* String literal */
        /* Do not reset to ommit the first quote */
        if (handle_string(&ret, &cur))
            return ret;
        /* TODO: lexical error */
        break;
    case '=':
        /* =: =, == */
        switch (*cur++) {
        case '=':
            /* == */
            OK_TOKEN(T_DEQ);
        default:
            /* = */
            OK_TOKEN(T_EQ);
        }
    case '!':
        /* !: != */
        switch (*cur++) {
        case '=':
            /* != */
            OK_TOKEN(T_NEQ);
        }
    case '+':
        /* +: +, += */
        switch (*cur++) {
        case '=':
            /* += */
            OK_TOKEN(T_PLUSEQ);
        default:
            /* + */
            OK_TOKEN(T_PLUS);
        }
    case '-':
        /* -: -, -= */
        switch (*cur++) {
        case '=':
            /* -= */
            OK_TOKEN(T_MINUSEQ);
        default:
            /* - */
            OK_TOKEN(T_MINUS);
        }
    case '*':
        /* *: *, *= */
        switch (*cur++) {
        case '=':
            /* *= */
            OK_TOKEN(T_STAREQ);
        default:
            /* * */
            OK_TOKEN(T_STAR);
        }
    case '/':
        /* /: /, /=, //, //= */
        switch (*cur++) {
        case '=':
            /* /= */
            OK_TOKEN(T_SLASHEQ);
        case '/':
            /* //: //, //= */
            switch (*cur++) {
            case '=':
                /* //= */
                OK_TOKEN(T_DSLASHEQ);
            default:
                /* // */
                OK_TOKEN(T_DSLASH);
            }
        default:
            /* / */
            OK_TOKEN(T_SLASH);
        }
    case '^':
        /* ^: ^, ^= */
        switch (*cur++) {
        case '=':
            /* ^= */
            OK_TOKEN(T_HATEQ);
        default:
            /* ^ */
            OK_TOKEN(T_HAT);
        }
    case '&':
        /* &: &, &= */
        switch (*cur++) {
        case '=':
            /* &= */
            OK_TOKEN(T_AMPEQ);
        default:
            /* & */
            OK_TOKEN(T_AMP);
        }
    case '<':
        /* <: <, <= */
        switch (*cur++) {
        case '=':
            /* <= */
            OK_TOKEN(T_LEQ);
        default:
            /* < */
            OK_TOKEN(T_LT);
        }
    case '>':
        switch (*cur++) {
        case '=':
            /* >= */
            OK_TOKEN(T_GEQ);
        default:
            /* > */
            OK_TOKEN(T_GT);
        }
    case '.':
        OK_TOKEN(T_DOT);
    case ',':
        OK_TOKEN(T_COMMA);
    case ':':
        OK_TOKEN(T_COLON);
    case ';':
        OK_TOKEN(T_SEMICOLON);
    case '@':
        OK_TOKEN(T_ATSYMBOL);
    case '(':
        OK_TOKEN(T_OPAR);
    case ')':
        OK_TOKEN(T_CPAR);
    case '[':
        OK_TOKEN(T_OBRACKET);
    case ']':
        OK_TOKEN(T_CBRACKET);
    case '{':
        OK_TOKEN(T_OBRACES);
    case '}':
        OK_TOKEN(T_CBRACES);
    }

    /* Lexical error */
    ret.ok = 0;
    ret.consumed = 0;
    ret.token = T_NULL;
    return ret;
}

/* Full lexical analysis */
Tokens get_tokens(const char *code) {
    buffer buf = NULL;
    Capture c;
    buffer_new(&buf, sizeof(Capture));
    c.ok = 0;
    lineno = 0; /* Set global state */

    do {
        c = next_token(code);
        switch (c.ok) {
        case 0:
            /* Lexical error */
            todo();
        case 1:
            /* Got a token */
            code += c.consumed;
            if (c.token == T_NULL)
                continue;
            buffer_push(buf, &c);
            break;
        }
    } while (c.ok != 2);

    buffer_shrink(buf);

#ifdef DEBUG_PRINT_TOKENS
    {
        Capture *x = NULL;
        assert(T_NTOKENS == sizeof(token_strings) / sizeof(const char *));
        for (buffer_iter(buf, Capture, x)) {
            printf("%s", token_strings[x->token]);
            switch (x->token) {
            case T_BOOL:
                printf("(%lu) ", x->info);
                break;
            case T_INT:
                printf("(%s) ", (const char *)(x->info));
                break;
            case T_STRING:
                printf("(%s) ", (const char *)(x->info));
                break;
            default:
                printf(" ");
            }
        }
        printf("\n");
    }
#endif

    return buf;
}
