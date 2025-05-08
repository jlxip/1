#include "semantics.h"
#include <ds/string.h>
#include <string.h>

/* my_function -> _O11my_function */
/* std.ds.vec.Vec.new -> _O3std2ds3vec3Vec3new */

char *mangle(Ctx *ctx, const char *name) {
    string ret = sc("_O");
    size_t i;

    for (i = 0; i < buffer_num(ctx->modstack); ++i) {
        const char *mod = *buffer_get(ctx->modstack, i, const char *);
        saddi(&ret, strlen(mod));
        saddc(&ret, mod);
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
