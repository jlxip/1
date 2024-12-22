#include "../easy.h"
#include "../src/Grammar.h"
#include <common.h>

static state rec_rerun(
    buffer dst, buffer src, size_t nsym, map cache, state s) {
    /* Go through each symbol */
    map row = *buffer_get(src, s, map); /* map<symbol, Entry> */
    map newrow = NULL;
    symbol sym;
    size_t ret = buffer_num(dst); /* Inserted index */

    if (map_has(cache, &s))
        return *map_get(cache, &s, state);

    map_new_symbol(&newrow, sizeof(Entry), hash_entry, equal_entry, copy_entry,
        destroy_entry);
    buffer_push(dst, &newrow);
    map_add(cache, &s, &ret);

    for (sym = 1; sym <= nsym; ++sym) {
        Entry entry;
        Entry newentry;

        if (!map_has(row, &sym))
            continue;

        entry = *map_get(row, &sym, Entry);
        switch (entry.type) {
        case ENTRY_ACCEPT:
            /* Accept has no info */
            /* fallthrough */
        case ENTRY_REDUCE:
            /* Reduce's info are productions which aren't related to table */
            map_add(newrow, &sym, &entry);
            break;

        case ENTRY_SHIFT:
            /* Shift's info is related to table */
            /* fallthrough */
        case ENTRY_GOTO:
            /* GOTO's too */
            newentry.type = entry.type;
            newentry.info = rec_rerun(dst, src, nsym, cache, entry.info);
            map_add(newrow, &sym, &newentry);
            break;
        }
    }

    return ret;
}

static buffer rerun(buffer table, size_t nsym) {
    buffer ret = NULL;
    map cache = NULL; /* map<state (original), state (copy)> */

    buffer_new(&ret, sizeof(map));
    map_new_state(&cache, sizeof(state), hash_state, equal_state, copy_state,
        destroy_state);

    /* Begin recursively with state 0 which must always be augmented start */
    rec_rerun(ret, table, nsym, cache, 0);
    map_out(&cache);
    return ret;
}

/* Returns whether a and b are isomorphic LALR tables */
static bool isomorphic(const Grammar *g, buffer a, buffer b) {
    /*
        The idea here is to convert a to c and b to d in a deterministic way.
        If c and d are equal, then a and b are isomorphic.
        Assumes state 0 is the first.
    */
    buffer c, d;
    size_t i;
    bool ret = false;

    if (buffer_num(a) != buffer_num(b))
        return false;

    c = rerun(a, g->nsym); /* buffer<map<symbol, Entry>> */
    d = rerun(b, g->nsym);

    for (i = 0; i < buffer_num(c); ++i)
        if (!equal_map(buffer_get(c, i, void), buffer_get(d, i, void)))
            goto out;
    ret = true;

out:
    for (i = 0; i < buffer_num(c); ++i)
        map_out(buffer_get(c, i, map));
    buffer_out(&c);
    for (i = 0; i < buffer_num(d); ++i)
        map_out(buffer_get(d, i, map));
    buffer_out(&d);
    return ret;
}

#define ADD_ENTRY(S, T, I)                                                     \
    do {                                                                       \
        Entry e = {T, I};                                                      \
        x = S;                                                                 \
        map_add(m, &x, &e);                                                    \
    } while (0)

/*
    Source: Dragon Book, hardcoded table is Figure 4.41

    (S' -> S)
    S -> CC
    C -> cC | d
*/

static const char *tokens[] = {"c", "d", NULL};
static const char *nts[] = {"S", "C", NULL};
static const char test[] = "S -> C C\n"
                           "C -> c C | d\n";

void test_lalr_table(void) {
    buffer b = NULL;
    map m = NULL;
    size_t x;
    Grammar *g;

    b = NULL;
    buffer_new(&b, sizeof(map));
    /* State 0 */
    m = NULL;
    map_new_symbol(
        &m, sizeof(Entry), hash_entry, equal_entry, copy_entry, destroy_entry);
    ADD_ENTRY(1, ENTRY_SHIFT, 3 /* 36 in book */);
    ADD_ENTRY(2, ENTRY_SHIFT, 4 /* 47 in book */);
    ADD_ENTRY(4, ENTRY_GOTO, 1);
    ADD_ENTRY(5, ENTRY_GOTO, 2);
    buffer_push(b, &m);
    /* State 1 */
    m = NULL;
    map_new_symbol(
        &m, sizeof(Entry), hash_entry, equal_entry, copy_entry, destroy_entry);
    ADD_ENTRY(6 /* $ */, ENTRY_ACCEPT, 0);
    buffer_push(b, &m);
    /* State 2 */
    m = NULL;
    map_new_symbol(
        &m, sizeof(Entry), hash_entry, equal_entry, copy_entry, destroy_entry);
    ADD_ENTRY(1, ENTRY_SHIFT, 3 /* 36 in book */);
    ADD_ENTRY(2, ENTRY_SHIFT, 4 /* 47 in book */);
    ADD_ENTRY(5, ENTRY_GOTO, 5);
    buffer_push(b, &m);
    /* State 3 (36 in book) */
    m = NULL;
    map_new_symbol(
        &m, sizeof(Entry), hash_entry, equal_entry, copy_entry, destroy_entry);
    ADD_ENTRY(1, ENTRY_SHIFT, 3 /* 36 in book */);
    ADD_ENTRY(2, ENTRY_SHIFT, 4 /* 47 in book */);
    ADD_ENTRY(5, ENTRY_GOTO, 6 /* 89 in book */);
    buffer_push(b, &m);
    /* State 4 (47 in book) */
    m = NULL;
    map_new_symbol(
        &m, sizeof(Entry), hash_entry, equal_entry, copy_entry, destroy_entry);
    ADD_ENTRY(1, ENTRY_REDUCE, 2 /* 3 in book */);
    ADD_ENTRY(2, ENTRY_REDUCE, 2 /* 3 in book */);
    ADD_ENTRY(6 /* $ */, ENTRY_REDUCE, 2 /* 3 in book */);
    buffer_push(b, &m);
    /* State 5 */
    m = NULL;
    map_new_symbol(
        &m, sizeof(Entry), hash_entry, equal_entry, copy_entry, destroy_entry);
    ADD_ENTRY(6 /* $ */, ENTRY_REDUCE, 0 /* 1 in book */);
    buffer_push(b, &m);
    /* State 6 (89 in book) */
    m = NULL;
    map_new_symbol(
        &m, sizeof(Entry), hash_entry, equal_entry, copy_entry, destroy_entry);
    ADD_ENTRY(1, ENTRY_REDUCE, 1 /* 2 in book */);
    ADD_ENTRY(2, ENTRY_REDUCE, 1 /* 2 in book */);
    ADD_ENTRY(6 /* $ */, ENTRY_REDUCE, 1 /* 2 in book */);
    buffer_push(b, &m);

    /* Compute table myself */
    g = grammar(tokens, nts, test, "S");
    Grammar_augment(g);
    Grammar_compute_firsts(g);
    Grammar_compute_collection(g);
    Grammar_compute_table(g);

    /* Compare */
    assert(isomorphic(g, g->table, b));

    for (x = 0; x < buffer_num(b); ++x)
        map_out(buffer_get(b, x, map));
    buffer_out(&b);
    Grammar_out(g);
    free(g);
}
