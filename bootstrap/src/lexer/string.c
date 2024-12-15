#include "lexer.h"
#include <stdlib.h>
#include <string.h>

/* Match strings: "hello\n" */

size_t match_string(Capture *ret, const char *cur) {
    /* First quote comes already consumed */
    const char *begin = cur;
    char *buf = NULL;
    size_t len;

    for (;;) {
        switch (*cur++) {
        case '\\':
            /* Escape */
            todo(); /* not feeling like it */
        case '"':
            /* End */
            len = --cur - begin;
            buf = malloc(len + 1);
            memcpy(buf, begin, len);
            buf[len] = '\0';

            --begin; /* Include first quote now */
            ++cur;   /* As well as last one */
            OK_TOKEN_INFO(T_STRING, (size_t)buf);
        case '\0':
            /* Unfinished string */
            todo();
            return 0;
        }
    }
}
