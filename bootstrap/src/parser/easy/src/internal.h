#include "Grammar.h"

void Grammar_augment(Grammar *g);
set FIRST(Grammar *g, symbol sym);
set FIRST_MANY(Grammar *g, const buffer syms);
set CLOSURE(Grammar *g, const Item *item);
set GOTO(Grammar *g, const set items, symbol sym);
void Grammar_compute_collection(Grammar *g);
size_t handle_conflict(
    const Grammar *g, state st, symbol sym, const Entry *old);
