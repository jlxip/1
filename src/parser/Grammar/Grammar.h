#ifndef PARSER_GRAMMAR_H
#define PARSER_GRAMMAR_H

/*
    This folder, Grammar, implements a neat LALR parser generator from scratch.
    I can't use yacc since this project needs to be 100% portable, and yacc
    has runtime libraries. Special thanks to Dragon Book.
*/

#include <common.h>
#include <ds/buffer.h>
#include <ds/set.h>

typedef struct {
    size_t lhs;
    buffer rhs;
} Production;

#define EPSILON 0

/*
    Symbols are assumed to be terminals + N_TOKENS + non-terminals
        - sym = 0             <==> Epsilon
        - 0 < sym < N_TOKENS  <==> Terminal (token)
        - sym = N_TOKENS      <==> Augmented start symbol
        - sym > N_TOKENS      <==> Non-terminal
*/
#define IS_TERMINAL(G, X) ((X) < ((G)->ntok))
#define IS_NON_TERMINAL(G, X) (!(IS_TERMINAL(X)))

typedef struct {
    buffer g;
    size_t ntok;
    size_t start;
    size_t augmented;
} Grammar;

/* Grammar.c */
void Grammar_new(Grammar *g, size_t ntok, size_t start);
void Grammar_add(Grammar *g, size_t lhs, size_t n, ...);
void Grammar_shrink(Grammar *g);
void Grammar_augment(Grammar *g);
void Grammar_out(Grammar *g);

/* functions.c */
set get_first(const Grammar *g, size_t symbol);
/*set get_follow(const Grammar *g, size_t symbol);*/

#endif
