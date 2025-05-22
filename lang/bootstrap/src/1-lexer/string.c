#include "lexer.h"
#include <stdlib.h>
#include <string.h>

/* Match strings: "hello\n" */

size_t match_string(Capture *ret, const char *cur) {
    /* First quote comes already consumed */
    const char *begin = cur;
    char *buf = NULL;
    size_t len;
    bool escape = false;

    for (;;) {
        switch (*cur++) {
        case '\\':
            /* Escape */
            escape = !escape;
            break;
        case '"':
            if (escape) {
                escape = false;
                break;
            }

            /* End */
            len = --cur - begin;
            buf = malloc(len + 1);
            memcpy(buf, begin, len);
            buf[len] = '\0';

            --begin; /* Include first quote now */
            ++cur;   /* As well as last one */
            OK_TOKEN_DATA(T_STRING, str, buf);
            break;
        case '\0':
            throw("unfinished string");
            break;
        default:
            escape = false;
        }
    }
}
