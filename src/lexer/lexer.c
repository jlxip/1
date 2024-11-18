#ifdef DEBUG_PRINT_TOKENS
#define CREATE_TOKEN_STRINGS
#endif

#include "lexer.h"
#include <buffer.h>
#include <error.h>

size_t lineno = 0;

static Capture next_token(const char *cur) {
    Capture ret;
    ret.lineno = lineno;
    ret.info = 0;

    /* Special cases */
    switch (*cur) {
    case 0:
        ret.ok = 2; /* end */
        ret.token = T_NULL;
        ret.consumed = 0;
        return ret;
    case '\n':
        ++lineno;
    case ' ':
    case '\t':
        ret.ok = 1;
        ret.consumed = 1;
        ret.token = T_NULL;
        return ret;
    }

    /* Actual matches */
    if (match_keyword(&ret, cur))
        return ret;
    if (match_id(&ret, cur))
        return ret;
    if (match_symbol(&ret, cur))
        return ret;

    /* Lexical error */
    ret.ok = 0;
    ret.consumed = 0;
    ret.token = T_NULL;
    return ret;
}

/* Full lexical analysis */
buffer get_tokens(const char *code) {
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
