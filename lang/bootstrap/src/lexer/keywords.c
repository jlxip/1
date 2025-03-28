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
    if (_match(&cur, P)) {                                                     \
        --cur;                                                                 \
        OK_TOKEN(T);                                                           \
    }                                                                          \
    break;
#define MATCH_REST_INFO(P, T, I)                                               \
    if (_match(&cur, P)) {                                                     \
        --cur;                                                                 \
        OK_TOKEN_INFO(T, I);                                                   \
    }                                                                          \
    break;

size_t match_keyword(Capture *ret, const char *cur) {
    const char *begin = cur;
    switch (*cur++) {
    case 'a':
        /* a: and */
        MATCH_REST("nd", T_AND);
    case 'b':
        /* b: bool, break */
        switch (*cur++) {
        case 'o':
            /* bo: bool */
            MATCH_REST("ol", T_KBOOL);
        case 'r':
            /* br: break */
            MATCH_REST("eak", T_BREAK);
        }
    case 'c':
        /* c: case, continue */
        switch (*cur++) {
        case 'a':
            /* ca: case */
            MATCH_REST("se", T_CASE);
        case 'o':
            /* co: continue */
            MATCH_REST("ntinue", T_CONTINUE);
        }
    case 'd':
        /* d: default */
        MATCH_REST("efault", T_DEFAULT);
    case 'e':
        /* e: elif, else */
        if (*cur++ == 'l') {
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
                case 's':
                    MATCH_REST_INFO("e", T_BOOL, 0);
                }
            }
        case 'l':
            /* fl: float */
            MATCH_REST("oat", T_KFLOAT);
        case 'n':
            /* fn: fn */
            MAYBE_KEYWORD(T_FN);
        case 'o':
            /* fo: for */
            MATCH_REST("r", T_FOR);
        }
    case 'i':
        /* i: impl, in, if */
        switch (*cur++) {
        case 'm':
            MATCH_REST("pl", T_IMPL);
        case 'n':
            MAYBE_KEYWORD(T_IN);
        case 'f':
            MAYBE_KEYWORD(T_IF);
        }
    case 'l':
        /* l: let */
        MATCH_REST("et", T_LET);
    case 'n':
        /* n: not */
        MATCH_REST("ot", T_NOT);
    case 'o':
        /* o: or */
        MATCH_REST("r", T_OR);
    case 'p':
        /* p: ptr */
        MATCH_REST("tr", T_KPTR);
    case 'r':
        /* r: return */
        MATCH_REST("eturn", T_RETURN);
    case 's':
        /* s: string, struct */
        if (*cur++ == 't') {
            if (*cur++ == 'r') {
                switch (*cur++) {
                case 'i':
                    MATCH_REST("ng", T_KSTRING);
                case 'u':
                    MATCH_REST("ct", T_STRUCT);
                }
            }
        }
    case 't':
        /* t: true */
        MATCH_REST_INFO("rue", T_BOOL, 1);
    case 'u':
        /* u: use */
        MATCH_REST("se", T_USE);
    case 'w':
        /* w: while, word */
        switch (*cur++) {
        case 'h':
            /* wh: while */
            MATCH_REST("ile", T_WHILE);
        case 'o':
            /* wo: word */
            MATCH_REST("rd", T_KWORD);
        }
    }

    return 0;
}
