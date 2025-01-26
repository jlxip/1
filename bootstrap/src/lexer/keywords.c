#include "lexer.h"

/* Match keywords: break, elif */

#define CAN_BE_KEYWORD(X) (IS_AZ(X) || IS_az(X))

#define MAYBE_KEYWORD(X)                                                       \
    if (!CAN_BE_KEYWORD(*cur))                                                 \
        OK_TOKEN(X);                                                           \
    break;

/* Check the rest of the token, in case there's only one match. Keywords only */
static size_t _match(const char **cur, const char *target) {
    for (;;) {
        char a = *(*cur)++;
        char b = *target++;

        /* End of target = probably match */
        if (!b) {
            if (!CAN_BE_KEYWORD(a))
                return 1;
            return 0;
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
        OK_TOKEN(T);                                                           \
    break;
#define MATCH_REST_INFO(P, T, I)                                               \
    if (_match(&cur, P))                                                       \
        OK_TOKEN_INFO(T, I);                                                   \
    break;

size_t match_keyword(Capture *ret, const char *cur) {
    const char *begin = cur;
    switch (*cur++) {
    case 'a':
        /* a: and, as */
        switch (*cur++) {
        case 'n':
            /* an: and */
            MATCH_REST("d", T_AND);
        case 's':
            /* as: as */
            MAYBE_KEYWORD(T_AS);
        }
    case 'b':
        /* b: break */
        MATCH_REST("reak", T_BREAK);
    case 'c':
        /* c: continue */
        MATCH_REST("ontinue", T_CONTINUE);
    case 'd':
        /* d: del */
        MATCH_REST("el", T_DEL);
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
    case 'f':
        /* f: false, fn, for, from */
        switch (*cur++) {
        case 'a':
            /* fa: false */
            MATCH_REST_INFO("lse", T_BOOL, 0);
        case 'n':
            /* fn: fn */
            MAYBE_KEYWORD(T_FN);
        case 'o':
            /* fo: for */
            MATCH_REST("r", T_FOR);
        case 'r':
            /* fr: from */
            MATCH_REST("om", T_FROM);
        }
    case 'i':
        /* i: in, is, if */
        switch (*cur++) {
        case 'n':
            MAYBE_KEYWORD(T_IN);
        case 's':
            MAYBE_KEYWORD(T_IS);
        case 'f':
            MAYBE_KEYWORD(T_IF);
        }
    case 'l':
        /* l: lambda */
        MATCH_REST("ambda", T_LAMBDA);
    case 'n':
        /* n: not */
        MATCH_REST("ot", T_NOT);
    case 'o':
        /* o: or */
        MATCH_REST("r", T_OR);
    case 'r':
        /* r: return */
        MATCH_REST("turn", T_RETURN);
    case 's':
        /* s: struct */
        MATCH_REST("truct", T_STRUCT);
    case 't':
        /* t: true */
        MATCH_REST_INFO("rue", T_BOOL, 1);
    case 'u':
        /* u: use */
        MATCH_REST("se", T_USE);
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
    }

    return 0;
}
