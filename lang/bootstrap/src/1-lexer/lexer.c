#ifdef DEBUG
#define GET_TOKEN_STRINGS
#endif

/* Hand-written lexer for this project */

#include "lexer.h"
#include <error.h>

size_t lineno = 0;

static bool comment(Capture *ret, const char *cur) {
    size_t consumed = 2;
    assert(*cur++ == '/');
    switch (*cur++) {
    case '/':
        /* Line comment, advance until \n */
        for (;;)
            switch (*cur++) {
            case '\0':
            case '\n':
                ret->consumed = consumed;
                ret->token.id = T_NULL;
                return true;
            default:
                ++consumed;
            }
        UNREACHABLE;
        break;
    case '*':
        /* Block comment, advance until end */
        for (;;)
            switch (*cur++) {
            case '\0':
                throw("unfinished block comment");
                return false;
            case '*':
                if (*cur == '/') {
                    /* Close block */
                    ret->consumed = consumed + 2;
                    ret->token.id = T_NULL;
                    return true;
                }
                /* fallthrough */
            default:
                ++consumed;
            }
        UNREACHABLE;
        break;
    default:
        return false;
    }

    UNREACHABLE;
}

static bool capture(Capture *ret, const char *cur) {
    ret->token.lineno = lineno;
    ret->token.data.word = 0;

    /* Special cases */
    switch (*cur) {
    case '\0':
        return false; /* end */
    case '/':
        if (comment(ret, cur))
            return true;
        break;
    case '\n':
        ++lineno;
        /* fallthrough */
    case ' ':
    case '\t':
        ret->consumed = 1;
        ret->token.id = T_NULL;
        return true;
    }

    /* Actual matches */
    if (match_keyword(ret, cur))
        return true;
    if (match_id(ret, cur))
        return true;
    if (match_symbol(ret, cur))
        return true;

    /* Lexical error */
    throwe("lexical error in line %lu", lineno);
    return false; /* cannot happen */
}

/* Full lexical analysis */
buffer get_tokens(const char *code) {
    buffer buf = NULL;
    Capture c;

    buffer_new(&buf, sizeof(Token));
    lineno = 1; /* Set global state */

    while (capture(&c, code)) {
        code += c.consumed;
        if (c.token.id == T_NULL)
            continue;
        buffer_push(buf, &c.token);
    }

    buffer_shrink(buf);

#ifdef DEBUG
    do {
        size_t i;
        assert(T_NTOKENS + 1 == sizeof(token_strings) / sizeof(const char *));
        printf("Printing tokens\n");
        for (i = 0; i < buffer_num(buf); ++i) {
            Token *x = buffer_get(buf, i, Token);
            printf("%s", token_strings[x->id]);
            switch (x->id) {
            case T_BOOL:
                printf(x->data.word ? "(true) " : "(false) ");
                break;
            case T_WORD:
            case T_STRING:
            case T_ID:
                printf("(%s) ", x->data.str);
                break;
            default:
                printf(" ");
            }
        }
        printf("\nEnd of tokens\n");
    } while (0);
#endif

    return buf;
}
