#include "Grammar.h"

void Grammar_augment(Grammar *g);
set FIRST(Grammar *g, symbol sym);
set FIRST_MANY(Grammar *g, const buffer syms);
set CLOSURE(Grammar *g, const Item *item, bool core);
set GOTO(Grammar *g, const set items, symbol sym, bool core);
void Grammar_compute_collection(Grammar *g);
