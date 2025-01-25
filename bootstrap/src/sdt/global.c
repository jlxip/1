#include "sdt.h"
#include <common.h>
#include <string.h>
#include <tokens.h>

void *sdt_relpath_dot(void **sub, size_t nsub) {
    (void)sub;
    (void)nsub;
    todo();
    return NULL;
}

/* RELATIVE_PATH -> MODULE */
void *sdt_relpath_direct(void **sub, size_t nsub) {
    (void)nsub;
    return *sub;
}

void *sdt_module_dot(void **sub, size_t nsub) {
    (void)sub;
    (void)nsub;
    todo();
    return NULL;
}

/* MODULE -> id */
void *sdt_module_direct(void **sub, size_t nsub) {
    Capture *capture = (Capture *)*sub;
    const char *orig;
    size_t len;
    char *ret;
    (void)nsub;

    orig = (const char *)(capture->info);
    len = strlen(orig);
    ret = malloc(len + 1);
    memcpy(ret, orig, len + 1);
    return ret;
}
