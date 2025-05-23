#include "Grammar.h"
#include <ds/string.h>

/* --- SERIALIZATION --- */

static void join(buffer a, buffer b) {
    size_t i;
    for (i = 0; i < buffer_num(b); ++i)
        buffer_push(a, buffer_get(b, i, uint8_t));
}

static void joink(buffer a, buffer b) {
    join(a, b);
    buffer_out(&b);
}

static buffer serialize_size_t(size_t a) {
    buffer ret = NULL;
    size_t i;
    buffer_new(&ret, sizeof(uint8_t));

    for (i = 0; i < sizeof(size_t); ++i) {
        uint8_t val = (a >> (8 * i)) & 0xFF;
        buffer_push(ret, &val);
    }

    return ret;
}

static buffer serialize_buffer_size_t(buffer a) {
    buffer ret = NULL;
    size_t i;
    buffer_new(&ret, sizeof(uint8_t));

    joink(ret, serialize_size_t(buffer_num(a)));
    for (i = 0; i < buffer_num(a); ++i)
        joink(ret, serialize_size_t(*buffer_get(a, i, size_t)));

    return ret;
}

static buffer serialize_production(const Production *p) {
    buffer ret = NULL;
    buffer_new(&ret, sizeof(uint8_t));

    joink(ret, serialize_size_t(p->lhs));
    joink(ret, serialize_buffer_size_t(p->rhs));
    joink(ret, serialize_size_t(p->hint));

    return ret;
}

static buffer serialize_entry(const Entry *x) {
    buffer ret = NULL;
    buffer_new(&ret, sizeof(uint8_t));

    joink(ret, serialize_size_t(x->type));
    joink(ret, serialize_size_t(x->info));

    return ret;
}

static buffer serialize_map_symbol_entry(map x) {
    buffer ret = NULL;
    map_iterator it;
    buffer_new(&ret, sizeof(uint8_t));

    joink(ret, serialize_size_t(map_num(x)));
    it = map_it_begin(x);
    while (!map_it_finished(&it)) {
        joink(ret, serialize_size_t(*map_it_get_key(&it, size_t)));
        joink(ret, serialize_entry(map_it_get_value(&it, Entry)));
        map_it_next(&it);
    }

    return ret;
}

buffer Grammar_serialize(const Grammar *g) {
    buffer ret = NULL;
    size_t i;
    buffer_new(&ret, sizeof(uint8_t));

    assert(g->table);

    /* g->g */
    joink(ret, serialize_size_t(buffer_num(g->g)));
    for (i = 0; i < buffer_num(g->g); ++i)
        joink(ret, serialize_production(buffer_get(g->g, i, Production)));

    /* ntok, nsym, start */
    joink(ret, serialize_size_t(g->ntok));
    joink(ret, serialize_size_t(g->nsym));
    joink(ret, serialize_size_t(g->start));

    /* g->table */
    joink(ret, serialize_size_t(buffer_num(g->table)));
    for (i = 0; i < buffer_num(g->table); ++i)
        joink(ret, serialize_map_symbol_entry(*buffer_get(g->table, i, map)));

    return ret;
}

/* --- DESERIALIZATION --- */

typedef struct {
    buffer in;
    size_t pos;
} Ctx;

#define EXPECT(N) assert(ctx->pos + (N) < buffer_num(ctx->in));

static uint8_t next(Ctx *ctx) {
    uint8_t ret;
    ret = *buffer_get(ctx->in, ctx->pos, uint8_t);
    ctx->pos++;
    return ret;
}

#define NEXT next(ctx)

static size_t deserialize_size_t(Ctx *ctx) {
    size_t ret = 0;
    size_t i;

    EXPECT(4);
    for (i = 0; i < sizeof(size_t); ++i)
        ret |= ((size_t)NEXT) << (8 * i);

    return ret;
}

static buffer deserialize_buffer_size_t(Ctx *ctx) {
    buffer ret = NULL;
    size_t n, i;
    buffer_new(&ret, sizeof(size_t));

    n = deserialize_size_t(ctx);
    for (i = 0; i < n; ++i) {
        size_t aux = deserialize_size_t(ctx);
        buffer_push(ret, &aux);
    }

    return ret;
}

static Production deserialize_production(Ctx *ctx) {
    Production ret;

    ret.lhs = deserialize_size_t(ctx);
    ret.rhs = deserialize_buffer_size_t(ctx);
    ret.hint = deserialize_size_t(ctx);

    return ret;
}

static buffer deserialize_g(Ctx *ctx) {
    buffer ret = NULL;
    size_t n, i;
    buffer_new(&ret, sizeof(Production));

    n = deserialize_size_t(ctx);
    for (i = 0; i < n; ++i) {
        Production aux = deserialize_production(ctx);
        buffer_push(ret, &aux);
    }

    return ret;
}

static Entry deserialize_entry(Ctx *ctx) {
    Entry ret;

    ret.type = deserialize_size_t(ctx);
    ret.info = deserialize_size_t(ctx);

    return ret;
}

static map deserialize_map_symbol_entry(Ctx *ctx) {
    map ret = NULL;
    size_t n, i;
    map_new_size_t(&ret, sizeof(Entry), NULL, NULL, NULL, NULL);

    n = deserialize_size_t(ctx);
    for (i = 0; i < n; ++i) {
        size_t sym;
        Entry entry;

        sym = deserialize_size_t(ctx);
        entry = deserialize_entry(ctx);
        map_add(ret, &sym, &entry);
    }

    return ret;
}

static buffer deserialize_table(Ctx *ctx) {
    buffer ret = NULL;
    size_t n, i;
    buffer_new(&ret, sizeof(map));

    n = deserialize_size_t(ctx);
    for (i = 0; i < n; ++i) {
        map x = deserialize_map_symbol_entry(ctx);
        buffer_push(ret, &x);
    }

    return ret;
}

Grammar *Grammar_deserialize(buffer in) {
    Grammar *ret;
    Ctx ctx;
    ret = calloc(1, sizeof(Grammar));

    ctx.in = in;
    ctx.pos = 0;

    ret->g = deserialize_g(&ctx);
    ret->ntok = deserialize_size_t(&ctx);
    ret->nsym = deserialize_size_t(&ctx);
    ret->start = deserialize_size_t(&ctx);
    ret->augmented = 1;
    ret->table = deserialize_table(&ctx);

    assert(ctx.pos == buffer_num(ctx.in));
    return ret;
}
