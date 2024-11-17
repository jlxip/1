#include <buffer.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *msg;
    size_t line;
} Error;

static buffer errors = NULL;

int sort_by_line(const void *va, const void *vb) {
    const Error *a = (const Error *)va, *b = (const Error *)vb;
    return a->line - b->line;
}

size_t print_all_errors(void) {
    size_t any = 0;
    Error *x = NULL;

    buffer_shrink(errors);

    /* Sort errors by line */
    buffer_sort(errors, sort_by_line);

    /* Print all errors */
    for (buffer_iter(errors, Error, x)) {
        printf("ERROR: L%lu: %s\n", x->line, x->msg);
        free(x->msg);
        any = 1;
    }

    buffer_out(&errors);
    return any;
}

void log_error(const char *msg, size_t msglen, size_t line) {
    Error error;

    if (msglen == 0)
        msglen = strlen(msg);

    error.msg = malloc(msglen);
    error.line = line;
    memcpy(error.msg, msg, msglen);

    if (!errors)
        buffer_new(&errors, sizeof(Error));
    buffer_push(errors, &error);
}
