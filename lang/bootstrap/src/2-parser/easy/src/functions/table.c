#include "../internal.h"

void Grammar_compile(Grammar *g) {
    size_t idx;

    if (!g->collection)
        Grammar_compute_collection(g);

    buffer_new(&g->table, sizeof(map));
    for (idx = 0; idx < buffer_num(g->collection); ++idx) {
        map entries = NULL; /* map<symbol, Entry> */
        map_iterator mit;
        set_iterator sit;

        map_new_symbol(&entries, sizeof(Entry), hash_entry, equal_entry,
            copy_entry, destroy_entry);

        /* Look through the GOTOs (shift and goto) */
        mit = map_it_begin(*buffer_get(g->gotos, idx, map));
        while (!map_it_finished(&mit)) {
            symbol sym = *map_it_get_key(&mit, symbol);
            state target = *map_it_get_value(&mit, state);
            Entry entry;

            if (IS_TERMINAL(g, sym) || sym == g->nsym /* $ */)
                entry.type = ENTRY_SHIFT;
            else
                entry.type = ENTRY_GOTO;
            entry.info = target;
            map_add(entries, &sym, &entry);

            map_it_next(&mit);
        }

        /* Look through the items (reduce) */
        sit = set_it_begin(*buffer_get(g->collection, idx, set));
        while (!set_it_finished(&sit)) {
            const Item *item = set_it_get(&sit, Item);
            if (item->dot == END_OF_PRODUCTION) {
                /* That's a reduce for each symbol in lookahead set */
                set_iterator lit = set_it_begin(item->look);
                while (!set_it_finished(&lit)) {
                    /* Something already there? */
                    symbol sym = *set_it_get(&lit, symbol);
                    Entry entry;

                    if (item->prod == 0 /* augmented start */) {
                        entry.type = ENTRY_ACCEPT;
                        entry.info = 0;
                    } else {
                        entry.type = ENTRY_REDUCE;
                        entry.info = item->prod;
                    }

                    if (map_has(entries, &sym)) {
                        const Entry *old = map_get(entries, &sym, Entry);
                        switch (handle_conflict(g, idx, sym, old, &entry)) {
                        case 0:
                            /* Prefer shift (keep old) */
                            break;
                        case 1:
                            /* Prefer reduce (put new) */
                            map_remove(entries, &sym);
                            map_add(entries, &sym, &entry);
                            break;
                        default:
                            UNREACHABLE;
                        }
                    } else {
                        map_add(entries, &sym, &entry);
                    }

                    set_it_next(&lit);
                }
            }

            set_it_next(&sit);
        }

        buffer_push(g->table, &entries);
    }
}
