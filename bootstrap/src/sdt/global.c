#include "sdt.h"
#include <common.h>
#include <string.h>
#include <tokens.h>

/* S -> USES GLOBALS */
void *sdt_program(void **sub) { NOT_IMPLEMENTED; }

/* USES -> USE USES */
void *sdt_uses_rec(void **sub) { NOT_IMPLEMENTED; }

/* USES -> USE */
void *sdt_uses_direct(void **sub) { NOT_IMPLEMENTED; }

/* USE -> use RELATIVE_PATH semicolon */
void *sdt_use(void **sub) {
    (void)sub;
    printf("Ignoring use for now...\n");
    free(sub[1]);
    return NULL;
}

/* RELATIVE_PATH -> dot RELATIVE_PATH */
void *sdt_relpath_dot(void **sub) { NOT_IMPLEMENTED; }

/* RELATIVE_PATH -> MODULE */
void *sdt_relpath_direct(void **sub) { return *sub; }

/* MODULE -> id dot MODULE */
void *sdt_module_dot(void **sub) { NOT_IMPLEMENTED; }

/* MODULE -> id */
void *sdt_module_direct(void **sub) {
    Capture *capture = (Capture *)*sub;
    const char *orig;
    size_t len;
    char *ret;

    orig = (const char *)(capture->info);
    len = strlen(orig);
    ret = malloc(len + 1);
    memcpy(ret, orig, len + 1);
    return ret;
}

/* GLOBALS -> GLOBAL GLOBALS */
void *sdt_globals_rec(void **sub) { NOT_IMPLEMENTED; }

/* GLOBALS -> GLOBAL */
void *sdt_globals_direct(void **sub) { NOT_IMPLEMENTED; }

/* GLOBAL -> ASSIGNMENT semicolon */
void *sdt_global_assign(void **sub) { NOT_IMPLEMENTED; }

/* GLOBAL -> FUNCTION */
void *sdt_global_func(void **sub) { NOT_IMPLEMENTED; }

/* GLOBAL -> STRUCT */
void *sdt_global_struct(void **sub) { NOT_IMPLEMENTED; }

/* FUNCTION -> fn id opar TYPED_IDS cpar BLOCK */
void *sdt_func_args(void **sub) { NOT_IMPLEMENTED; }

/* TYPED_IDS -> TYPED_ID comma TYPED_IDS */
void *sdt_typed_ids_rec(void **sub) { NOT_IMPLEMENTED; }

/* TYPED_IDS -> EPSILON */
void *sdt_typed_ids_null(void **sub) { NOT_IMPLEMENTED; }
