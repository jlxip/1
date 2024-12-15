#include "lexer.h"
#include <stdlib.h>
#include <string.h>

/* Match IDs: my_variable1 */

size_t match_id(Capture *ret, const char *cur) {
    const char *begin = cur;
    char *buf = NULL; /* Copied in case of success */

    /* First character must be [A-Za-z_] */
    char c = *cur++;
    if (!(IS_AZ(c) || IS_az(c) || IS__(c)))
        return 0;

    /* Next characters are in [A-Za-z0-9_] */
    for (;;) {
        c = *cur++;
        if (!(IS_AZ(c) || IS_az(c) || IS_09(c) || IS__(c))) {
            size_t len = --cur - begin;
            buf = malloc(len + 1);
            memcpy(buf, begin, len);
            buf[len] = '\0';
            OK_TOKEN_INFO(T_ID, (size_t)buf);
        }
    }
}
