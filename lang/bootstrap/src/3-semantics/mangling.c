#include "semantics.h"
#include <ds/string.h>
#include <string.h>

/* my_function -> _X11my_function */
/* std.ds.vec.Vec.new -> _X3std2ds3vec3Vec3new */

char *mangle(Ctx *ctx, const char *name) {
    string ret = sc("_X");
    size_t i;

    for (i = 0; i < buffer_num(ctx->mangling_stack); ++i) {
        const char *scope = *buffer_get(ctx->mangling_stack, i, const char *);
        saddi(&ret, strlen(scope));
        saddc(&ret, scope);
    }

    saddi(&ret, strlen(name));
    saddc(&ret, name);

    return sget(ret);
}

char *mangle_var(Ctx *ctx, const char *name) {
    string ret = sc(name);
    ShadowingTable shadow = *buffer_back(ctx->shadowing, ShadowingTable);

    if (map_has(shadow, name)) {
        size_t ctr = *map_get(shadow, name, size_t);
        saddi(&ret, ctr);
    }

    return sget(ret);
}
