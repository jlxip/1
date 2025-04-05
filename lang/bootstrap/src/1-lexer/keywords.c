#include "lexer.h"

/* Match keywords: break, elif */

#define CAN_BE_KEYWORD(X) (IS_AZ(X) || IS_az(X))

#define MAYBE_KEYWORD(X)                                                       \
    if (!CAN_BE_KEYWORD(*cur))                                                 \
        OK_TOKEN(X);

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
#define MATCH_REST(P, X)                                                       \
    if (_match(&cur, P)) {                                                     \
        --cur;                                                                 \
        OK_TOKEN(X);                                                           \
    }
#define MATCH_REST_DATA(P, X, T, I)                                            \
    if (_match(&cur, P)) {                                                     \
        --cur;                                                                 \
        OK_TOKEN_DATA(X, T, I);                                                \
    }

size_t match_keyword(Capture *ret, const char *cur) {
    const char *begin = cur;
    switch (*cur++) {
    case 'a':
        /* a: and */
        MATCH_REST("nd", T_AND);
        return 0;
    case 'b':
        /* b: bool, break, byte */
        switch (*cur++) {
        case 'o':
            /* bo: bool */
            MATCH_REST("ol", T_KBOOL);
            return 0;
        case 'r':
            /* br: break */
            MATCH_REST("eak", T_BREAK);
            return 0;
        case 'y':
            /* by: byte */
            MATCH_REST("te", T_KBYTE);
            return 0;
        }
        return 0;
    case 'c':
        /* c: case, continue */
        switch (*cur++) {
        case 'a':
            /* ca: case */
            MATCH_REST("se", T_CASE);
            return 0;
        case 'o':
            /* co: continue */
            MATCH_REST("ntinue", T_CONTINUE);
            return 0;
        }
        return 0;
    case 'd':
        /* d: default */
        MATCH_REST("efault", T_DEFAULT);
        return 0;
    case 'e':
        /* e: elif, else, extern */
        switch (*cur++) {
        case 'l':
            /* el: elif, else */
            switch (*cur++) {
            case 'i':
                /* eli: elif */
                MATCH_REST("f", T_ELIF);
                return 0;
            case 's':
                /* els: else */
                MATCH_REST("e", T_ELSE);
                return 0;
            }
            return 0;
        case 'x':
            /* ex: extern */
            MATCH_REST("tern", T_EXTERN);
            return 0;
        }
        return 0;
    case 'f':
        /* f: fall, false, float, fn, for */
        switch (*cur++) {
        case 'a':
            /* fa: fall, false */
            if (*cur++ == 'l') {
                /* fal: fall, false */
                switch (*cur++) {
                case 'l':
                    /* fall */
                    MAYBE_KEYWORD(T_FALL);
                    return 0;
                case 's':
                    MATCH_REST_DATA("e", T_BOOL, word, 0);
                    return 0;
                }
                return 0;
            }
            return 0;
        case 'l':
            /* fl: float */
            MATCH_REST("oat", T_KFLOAT);
            return 0;
        case 'n':
            /* fn: fn */
            MAYBE_KEYWORD(T_FN);
            return 0;
        case 'o':
            /* fo: for */
            MATCH_REST("r", T_FOR);
            return 0;
        }
        return 0;
    case 'i':
        /* i: impl, in, if */
        switch (*cur++) {
        case 'm':
            MATCH_REST("pl", T_IMPL);
            return 0;
        case 'n':
            MAYBE_KEYWORD(T_IN);
            return 0;
        case 'f':
            MAYBE_KEYWORD(T_IF);
            return 0;
        }
        return 0;
    case 'l':
        /* l: let */
        MATCH_REST("et", T_LET);
        return 0;
    case 'n':
        /* n: not */
        MATCH_REST("ot", T_NOT);
        return 0;
    case 'o':
        /* o: or */
        MATCH_REST("r", T_OR);
        return 0;
    case 'p':
        /* p: ptr */
        MATCH_REST("tr", T_KPTR);
        return 0;
    case 'r':
        /* r: return */
        MATCH_REST("eturn", T_RETURN);
        return 0;
    case 's':
        /* s: sizeof, string, struct */
        switch (*cur++) {
        case 'i':
            /* si: sizeof */
            MATCH_REST("zeof", T_SIZEOF);
            return 0;
        case 't':
            /* st: string, struct */
            if (*cur++ == 'r') {
                switch (*cur++) {
                case 'i':
                    MATCH_REST("ng", T_KSTRING);
                    return 0;
                case 'u':
                    MATCH_REST("ct", T_STRUCT);
                    return 0;
                }
                return 0;
            }
            return 0;
        }
        return 0;
    case 't':
        /* t: true */
        MATCH_REST_DATA("rue", T_BOOL, word, 1);
        return 0;
    case 'u':
        /* u: use */
        MATCH_REST("se", T_USE);
        return 0;
    case 'w':
        /* w: while, word */
        switch (*cur++) {
        case 'h':
            /* wh: while */
            MATCH_REST("ile", T_WHILE);
            return 0;
        case 'o':
            /* wo: word */
            MATCH_REST("rd", T_KWORD);
            return 0;
        }
        return 0;
    default:
        return 0;
    }

    /* This unreachable is just to make sure that nothing falls through */
    UNREACHABLE;
    return 0;
}
