#include "../easy.h"
#include "../src/internal.h"
#include "grammars.h"
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

/*static void print_table(buffer table, size_t nsym) {
    size_t x;
    for (x = 0; x < buffer_num(table); ++x) {
        map m = *buffer_get(table, x, map);
        size_t i;
        printf("%lu -> ", x);
        for (i = 1; i <= nsym; ++i) {
            if (map_has(m, &i)) {
                Entry entry = *map_get(m, &i, Entry);
                if (entry.type == ENTRY_SHIFT)
                    printf("s%lu", entry.info);
                else if (entry.type == ENTRY_GOTO)
                    printf("%lu ", entry.info);
                else if (entry.type == ENTRY_REDUCE)
                    printf("r%lu", entry.info);
                else if (entry.type == ENTRY_ACCEPT)
                    printf("ac");
            } else {
                printf("  ");
            }

            if (i != nsym)
                printf(" ");
        }
        printf("\n");
    }
    printf("\n");
}*/

#define ADD_ENTRY(S, T, I)                                                     \
    do {                                                                       \
        Entry e = {T, I};                                                      \
        x = S;                                                                 \
        map_add(m, &x, &e);                                                    \
    } while (0)

static void test_lalr_table0(void) {
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
    ADD_ENTRY(1, ENTRY_REDUCE, 3);
    ADD_ENTRY(2, ENTRY_REDUCE, 3);
    ADD_ENTRY(6 /* $ */, ENTRY_REDUCE, 3);
    buffer_push(b, &m);
    /* State 5 */
    m = NULL;
    map_new_symbol(
        &m, sizeof(Entry), hash_entry, equal_entry, copy_entry, destroy_entry);
    ADD_ENTRY(6 /* $ */, ENTRY_REDUCE, 1);
    buffer_push(b, &m);
    /* State 6 (89 in book) */
    m = NULL;
    map_new_symbol(
        &m, sizeof(Entry), hash_entry, equal_entry, copy_entry, destroy_entry);
    ADD_ENTRY(1, ENTRY_REDUCE, 2);
    ADD_ENTRY(2, ENTRY_REDUCE, 2);
    ADD_ENTRY(6 /* $ */, ENTRY_REDUCE, 2);
    buffer_push(b, &m);

    /* Compute table myself */
    g = grammar(tokens_basic, nts_basic, grammar_basic, "S");
    Grammar_compute_collection(g);
    Grammar_compile(g);

    /* Compare */
    assert(isomorphic(g, g->table, b));

    for (x = 0; x < buffer_num(b); ++x)
        map_out(buffer_get(b, x, map));
    buffer_out(&b);
    Grammar_out(g);
    free(g);
}

#define BEGIN_STATE                                                            \
    do {                                                                       \
        m = NULL;                                                              \
        map_new_symbol(&m, sizeof(Entry), hash_entry, equal_entry, copy_entry, \
            destroy_entry);                                                    \
    } while (0)
#define END_STATE buffer_push(b, &m);

static void test_lalr_table1(void) {
    buffer b = NULL;
    map m = NULL;
    size_t x;
    Grammar *g;

    b = NULL;
    buffer_new(&b, sizeof(map));
    /* State 0 */
    BEGIN_STATE;
    ADD_ENTRY(3, ENTRY_SHIFT, 5 /* 512 in slide */);
    ADD_ENTRY(2, ENTRY_SHIFT, 4 /* 411 in slide */);
    ADD_ENTRY(5, ENTRY_GOTO, 1);
    ADD_ENTRY(6, ENTRY_GOTO, 2);
    ADD_ENTRY(7, ENTRY_GOTO, 3);
    END_STATE;
    /* State 1 */
    BEGIN_STATE;
    ADD_ENTRY(8 /* $ */, ENTRY_ACCEPT, 0);
    END_STATE;
    /* State 2 */
    BEGIN_STATE;
    ADD_ENTRY(1, ENTRY_SHIFT, 6);
    ADD_ENTRY(8 /* $ */, ENTRY_REDUCE, 5);
    END_STATE;
    /* State 3 */
    BEGIN_STATE;
    ADD_ENTRY(8 /* $ */, ENTRY_REDUCE, 2);
    END_STATE;
    /* State 4 (411 in slide) */
    BEGIN_STATE;
    ADD_ENTRY(3, ENTRY_SHIFT, 5 /* 512 in slide */);
    ADD_ENTRY(2, ENTRY_SHIFT, 4 /* 411 in slide */);
    ADD_ENTRY(6, ENTRY_GOTO, 8 /* 810 in slide */);
    ADD_ENTRY(7, ENTRY_GOTO, 7 /* 713 in slide */);
    END_STATE;
    /* State 5 (512 in slide) */
    BEGIN_STATE;
    ADD_ENTRY(1, ENTRY_REDUCE, 4);
    ADD_ENTRY(8 /* $ */, ENTRY_REDUCE, 4);
    END_STATE;
    /* State 6 */
    BEGIN_STATE;
    ADD_ENTRY(3, ENTRY_SHIFT, 5 /* 512 in slide */);
    ADD_ENTRY(2, ENTRY_SHIFT, 4 /* 411 in slide */);
    ADD_ENTRY(6, ENTRY_GOTO, 8 /* 810 in slide */);
    ADD_ENTRY(7, ENTRY_GOTO, 9);
    END_STATE;
    /* State 7 (713 in slide) */
    BEGIN_STATE;
    ADD_ENTRY(1, ENTRY_REDUCE, 3);
    ADD_ENTRY(8 /* $ */, ENTRY_REDUCE, 3);
    END_STATE;
    /* State 8 (810 in slide) */
    BEGIN_STATE;
    ADD_ENTRY(1, ENTRY_REDUCE, 5);
    ADD_ENTRY(8 /* $ */, ENTRY_REDUCE, 5);
    END_STATE;
    /* State 9 */
    BEGIN_STATE;
    ADD_ENTRY(8 /* $ */, ENTRY_REDUCE, 1);
    END_STATE;

    /* Compute table myself */
    g = grammar(tokens_lr, nts_lr, grammar_lr, "S");
    Grammar_compute_collection(g);
    Grammar_compile(g);

    /* Compare */
    assert(isomorphic(g, g->table, b));

    for (x = 0; x < buffer_num(b); ++x)
        map_out(buffer_get(b, x, map));
    buffer_out(&b);
    Grammar_out(g);
    free(g);
}

static void test_lalr_table2(void) {
    buffer b = NULL;
    map m = NULL;
    size_t x;
    Grammar *g;

    b = NULL;
    buffer_new(&b, sizeof(map));
    /* State 0 */
    BEGIN_STATE;
    ADD_ENTRY(3, ENTRY_SHIFT, 4);
    ADD_ENTRY(4, ENTRY_SHIFT, 5);
    ADD_ENTRY(6, ENTRY_GOTO, 1);
    ADD_ENTRY(7, ENTRY_GOTO, 2);
    ADD_ENTRY(8, ENTRY_GOTO, 3);
    END_STATE;
    /* State 1 */
    BEGIN_STATE;
    ADD_ENTRY(1, ENTRY_SHIFT, 6);
    ADD_ENTRY(9 /* $ */, ENTRY_ACCEPT, 0);
    END_STATE;
    /* State 2 */
    BEGIN_STATE;
    ADD_ENTRY(1, ENTRY_REDUCE, 2);
    ADD_ENTRY(3, ENTRY_SHIFT, 4);
    ADD_ENTRY(4, ENTRY_SHIFT, 5);
    ADD_ENTRY(9 /* $ */, ENTRY_REDUCE, 2);
    ADD_ENTRY(8, ENTRY_GOTO, 7);
    END_STATE;
    /* State 3 */
    BEGIN_STATE;
    ADD_ENTRY(1, ENTRY_REDUCE, 4);
    ADD_ENTRY(2, ENTRY_SHIFT, 8);
    ADD_ENTRY(3, ENTRY_REDUCE, 4);
    ADD_ENTRY(4, ENTRY_REDUCE, 4);
    ADD_ENTRY(9 /* $ */, ENTRY_REDUCE, 4);
    END_STATE;
    /* State 4 */
    BEGIN_STATE;
    ADD_ENTRY(1, ENTRY_REDUCE, 6);
    ADD_ENTRY(2, ENTRY_REDUCE, 6);
    ADD_ENTRY(3, ENTRY_REDUCE, 6);
    ADD_ENTRY(4, ENTRY_REDUCE, 6);
    ADD_ENTRY(9 /* $ */, ENTRY_REDUCE, 6);
    END_STATE;
    /* State 5 */
    BEGIN_STATE;
    ADD_ENTRY(1, ENTRY_REDUCE, 7);
    ADD_ENTRY(2, ENTRY_REDUCE, 7);
    ADD_ENTRY(3, ENTRY_REDUCE, 7);
    ADD_ENTRY(4, ENTRY_REDUCE, 7);
    ADD_ENTRY(9 /* $ */, ENTRY_REDUCE, 7);
    END_STATE;
    /* State 6 */
    BEGIN_STATE;
    ADD_ENTRY(3, ENTRY_SHIFT, 4);
    ADD_ENTRY(4, ENTRY_SHIFT, 5);
    ADD_ENTRY(7, ENTRY_GOTO, 9);
    ADD_ENTRY(8, ENTRY_GOTO, 3);
    END_STATE;
    /* State 7 */
    BEGIN_STATE;
    ADD_ENTRY(1, ENTRY_REDUCE, 3);
    ADD_ENTRY(2, ENTRY_SHIFT, 8);
    ADD_ENTRY(3, ENTRY_REDUCE, 3);
    ADD_ENTRY(4, ENTRY_REDUCE, 3);
    ADD_ENTRY(9 /* $ */, ENTRY_REDUCE, 3);
    END_STATE;
    /* State 8 */
    BEGIN_STATE;
    ADD_ENTRY(1, ENTRY_REDUCE, 5);
    ADD_ENTRY(2, ENTRY_REDUCE, 5);
    ADD_ENTRY(3, ENTRY_REDUCE, 5);
    ADD_ENTRY(4, ENTRY_REDUCE, 5);
    ADD_ENTRY(9 /* $ */, ENTRY_REDUCE, 5);
    END_STATE;
    /* State 9 */
    BEGIN_STATE;
    ADD_ENTRY(1, ENTRY_REDUCE, 1);
    ADD_ENTRY(3, ENTRY_SHIFT, 4);
    ADD_ENTRY(4, ENTRY_SHIFT, 5);
    ADD_ENTRY(9 /* $ */, ENTRY_REDUCE, 1);
    ADD_ENTRY(8, ENTRY_GOTO, 7);
    END_STATE;

    /* Compute table myself */
    g = grammar(tokens_random, nts_random, grammar_random, "S");
    Grammar_compute_collection(g);
    Grammar_compile(g);

    /* Compare */
    assert(isomorphic(g, g->table, b));

    for (x = 0; x < buffer_num(b); ++x)
        map_out(buffer_get(b, x, map));
    buffer_out(&b);
    Grammar_out(g);
    free(g);
}

void test_lalr_table_wtf(void) {
    buffer b = NULL;
    map m = NULL;
    size_t x;
    Grammar *g;

    b = NULL;
    buffer_new(&b, sizeof(map));
    /* State 0 */
    BEGIN_STATE;
    ADD_ENTRY(1, ENTRY_SHIFT, 4);
    ADD_ENTRY(6, ENTRY_REDUCE, 4);
    ADD_ENTRY(3, ENTRY_SHIFT, 5);
    ADD_ENTRY(5, ENTRY_SHIFT, 6);
    ADD_ENTRY(8, ENTRY_GOTO, 1);
    ADD_ENTRY(9, ENTRY_GOTO, 2);
    ADD_ENTRY(11, ENTRY_GOTO, 3);
    END_STATE;
    /* State 1 */
    BEGIN_STATE;
    ADD_ENTRY(12 /* $ */, ENTRY_ACCEPT, 0);
    END_STATE;
    /* State 2 */
    BEGIN_STATE;
    ADD_ENTRY(6, ENTRY_SHIFT, 8);
    ADD_ENTRY(10, ENTRY_GOTO, 7);
    END_STATE;
    /* State 3 */
    BEGIN_STATE;
    ADD_ENTRY(12 /* $ */, ENTRY_REDUCE, 2);
    END_STATE;
    /* State 4 */
    BEGIN_STATE;
    ADD_ENTRY(1, ENTRY_SHIFT, 4);
    ADD_ENTRY(6, ENTRY_REDUCE, 4);
    ADD_ENTRY(9, ENTRY_GOTO, 9);
    END_STATE;
    /* State 5 */
    BEGIN_STATE;
    ADD_ENTRY(3, ENTRY_SHIFT, 5);
    ADD_ENTRY(5, ENTRY_SHIFT, 6);
    ADD_ENTRY(11, ENTRY_GOTO, 10);
    END_STATE;
    /* State 6 */
    BEGIN_STATE;
    ADD_ENTRY(4, ENTRY_REDUCE, 8);
    ADD_ENTRY(12 /* $ */, ENTRY_REDUCE, 8);
    END_STATE;
    /* State 7 */
    BEGIN_STATE;
    ADD_ENTRY(2, ENTRY_SHIFT, 11);
    ADD_ENTRY(12 /* $ */, ENTRY_REDUCE, 1);
    END_STATE;
    /* State 8 */
    BEGIN_STATE;
    ADD_ENTRY(2, ENTRY_REDUCE, 6);
    ADD_ENTRY(12 /* $ */, ENTRY_REDUCE, 6);
    END_STATE;
    /* State 9 */
    BEGIN_STATE;
    ADD_ENTRY(6, ENTRY_REDUCE, 3);
    END_STATE;
    /* State 10 */
    BEGIN_STATE;
    ADD_ENTRY(4, ENTRY_SHIFT, 12);
    END_STATE;
    /* State 11 */
    BEGIN_STATE;
    ADD_ENTRY(2, ENTRY_REDUCE, 5);
    ADD_ENTRY(12 /* $ */, ENTRY_REDUCE, 5);
    END_STATE;
    /* State 12 */
    BEGIN_STATE;
    ADD_ENTRY(4, ENTRY_REDUCE, 7);
    ADD_ENTRY(12 /* $ */, ENTRY_REDUCE, 7);
    END_STATE;

    /* Compute table myself */
    g = grammar(tokens_wtf, nts_wtf, grammar_wtf, "S");
    Grammar_compute_collection(g);
    Grammar_compile(g);

    /* Compare */
    assert(isomorphic(g, g->table, b));

    for (x = 0; x < buffer_num(b); ++x)
        map_out(buffer_get(b, x, map));
    buffer_out(&b);
    Grammar_out(g);
    free(g);
}

void test_lalr_table(void) {
    test_lalr_table0();
    test_lalr_table1();
    test_lalr_table2();
    test_lalr_table_wtf();
}
