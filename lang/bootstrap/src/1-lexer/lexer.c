#ifdef DEBUG
#define GET_TOKEN_STRINGS
#endif

/* Hand-written lexer for this project */

#include "lexer.h"
#include <error.h>

size_t lineno = 0;

static bool capture(Capture *ret, const char *cur) {
    ret->token.lineno = lineno;
    ret->token.data.word = 0;

    /* Special cases */
    switch (*cur) {
    case 0:
        return false; /* end */
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
        return 1;
    if (match_id(ret, cur))
        return 1;
    if (match_symbol(ret, cur))
        return 1;

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
