#include "lexer.h"
#include <stdlib.h>
#include <string.h>

/* Match numeric literals */

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

static size_t atow(const char *str, size_t len) {
    size_t i;
    size_t ret = 0;
    for (i = 0; i < len; ++i) {
        ret *= 10;
        ret += str[i] - '0';
    }
    return ret;
}

size_t match_numeric(Capture *ret, const char *cur) {
    const char *begin = cur;
    char *buf = NULL;
    double val;
    size_t len = 0;

    /* State 0: only numbers */
    for (;;) {
        switch (*cur++) {
        CASE_NUMBERS:
            break;
        case '.':
            goto state1;
        case 'e':
            goto state2;
        default:
            /* It's a word! */
            len = --cur - begin;
            OK_TOKEN_DATA(T_WORD, word, atow(begin, len));
        }
    }

state1:
    /* State 1: a dot has appeared, must be float, but no more dots! */
    for (;;) {
        switch (*cur++) {
        CASE_NUMBERS:
            break;
        case 'e':
            goto state2;
        default:
            /* It's a float! */
            len = --cur - begin;
            buf = malloc(len + 1);
            memcpy(buf, begin, len);
            buf[len] = '\0';
            val = atof(buf);
            free(buf);
            OK_TOKEN_DATA(T_FLOAT, d, val);
        }
    }

state2:
    /* State 2: an 'e' has appeared, must be float, but no more symbols! */
    for (;;) {
        switch (*cur++) {
        CASE_NUMBERS:
            break;
        default:
            /* It's a float! */
            len = --cur - begin;
            buf = malloc(len + 1);
            memcpy(buf, begin, len);
            buf[len] = '\0';
            val = atof(buf);
            free(buf);
            OK_TOKEN_DATA(T_FLOAT, d, val);
        }
    }
}
