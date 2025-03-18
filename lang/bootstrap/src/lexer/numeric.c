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

size_t match_numeric(Capture *ret, const char *cur) {
    const char *begin = cur;
    char *buf = NULL;   /* In case of word */
    double *val = NULL; /* In case of float */
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
            buf = malloc(len + 1);
            memcpy(buf, begin, len);
            buf[len] = '\0';
            OK_TOKEN_INFO(T_WORD, (size_t)buf);
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
            val = malloc(sizeof(double));
            *val = atof(buf);
            free(buf);
            OK_TOKEN_INFO(T_FLOAT, (size_t)val);
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
            val = malloc(sizeof(double));
            *val = atof(buf);
            free(buf);
            OK_TOKEN_INFO(T_FLOAT, (size_t)val);
        }
    }
}
