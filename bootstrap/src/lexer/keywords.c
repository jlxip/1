#include "lexer.h"

/* Match keywords: break, elif */

#define CAN_BE_KEYWORD(X) (IS_AZ(X) || IS_az(X))

#define MAYBE_KEYWORD(X)                                                       \
    do {                                                                       \
        if (!CAN_BE_KEYWORD(*cur++))                                           \
            OK_TOKEN(X);                                                       \
    } while (0)

/* Check the rest of the token, in case there's only one match. Keywords only */
static size_t _match(const char **cur, const char *target) {
    for (;;) {
        char a = *(*cur)++;
        char b = *target++;

        /* End of target = probably match */
        if (!b) {
            if (!CAN_BE_KEYWORD(a)) {
                --(*cur); /* go back */
                return 1;
            }
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
        OK_TOKEN(T);
#define MATCH_REST_INFO(P, T, I)                                               \
    if (_match(&cur, P))                                                       \
        OK_TOKEN_INFO(T, I);

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
                MAYBE_KEYWORD(T_DEF);
            case 'l':
                /* del: del */
                MAYBE_KEYWORD(T_DEL);
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
            MAYBE_KEYWORD(T_IN);
        case 's':
            MAYBE_KEYWORD(T_IS);
        case 'f':
            MAYBE_KEYWORD(T_IF);
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
    }

    return 0;
}
