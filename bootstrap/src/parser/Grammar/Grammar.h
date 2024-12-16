#ifndef PARSER_GRAMMAR_H
#define PARSER_GRAMMAR_H

/*
    This folder, Grammar, implements a neat LALR parser generator from scratch.
    I can't use yacc since this project needs to be 100% portable, and yacc
    has runtime libraries. Special thanks to Dragon Book.
*/

#include <common.h>
#include <ds/buffer.h>
#include <ds/map.h>
#include <ds/set.h>

typedef struct {
    size_t lhs;
    buffer rhs; /* buffer<size_t> */
} Production;

#define EPSILON 0

/*
    Symbols are assumed to be terminals + N_TOKENS + non-terminals + N_SYMBOLS
        - sym = 0                 <==> Epsilon
        - 0 < sym < N_TOKENS      <==> Terminal (token)
        - sym = N_TOKENS          <==> Augmented start symbol
        - N_TOKENS < sym < N_SYM  <==> Non-terminal
        - sym = N_SYMBOLS         <==> $
*/
#define IS_TERMINAL(G, X) ((X) < ((G)->ntok))
#define IS_NON_TERMINAL(G, X) (!(IS_TERMINAL(X)))

typedef struct {
    buffer g; /* buffer<Production> */
    size_t ntok;
    size_t nsym;
    size_t start;
    size_t augmented;
    map firsts;        /* map<size_t, set<size_t>> */
    map epsilons;      /* map<size_t, size_t> */
    map follows;       /* map<size_t, set<size_t>> */
    buffer collection; /* buffer<set<Item>> */
} Grammar;

/* LR(1) item */
typedef struct {
    size_t prod; /* index for Grammar.g */
    size_t dot;  /* position before symbol in rhs */
    set look;    /* second component, lookahead symbols: set<size_t> */
} Item;
/* Value of "dot" when it's at the end of the production */
#define END_OF_PRODUCTION (~0ul)
Item Item_new(size_t prod, size_t dot, size_t nlook, ...);
size_t hash_item(const void *ptr);
size_t hash_item_core(const void *ptr);
size_t equal_item(const void *a, const void *b);
size_t equal_item_core(const void *a, const void *b);
void *copy_item(const void *a);
void destroy_item(void *a);
#define set_new_Item(S)                                                        \
    set_new(S, hash_item, equal_item, copy_item, destroy_item)
#define set_new_Item_core(S)                                                   \
    set_new(S, hash_item_core, equal_item_core, copy_item, destroy_item)

/* Grammar.c */
void Grammar_new(Grammar *g, size_t ntok, size_t nsym, size_t start);
void Grammar_add(Grammar *g, size_t lhs, size_t n, ...);
void Grammar_shrink(Grammar *g);
void Grammar_augment(Grammar *g);
void Grammar_out(Grammar *g);

/* functions */
void Grammar_compute_firsts(Grammar *g);
set Grammar_first_many(const Grammar *g, const buffer syms);
void Grammar_compute_follows(Grammar *g);
set Grammar_closure(const Grammar *g, const Item *item, size_t core);
set Grammar_goto(const Grammar *g, const set items, size_t sym, size_t core);
void Grammar_compute_collection(Grammar *g);

#endif
