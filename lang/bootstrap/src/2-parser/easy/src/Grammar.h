#ifndef PARSER_GRAMMAR_H
#define PARSER_GRAMMAR_H

/*
    This folder, Grammar, implements a neat LALR parser generator from scratch.
    I can't use yacc since this project needs to be 100% portable, and yacc
    has runtime libraries. Special thanks to the Dragon Book.
*/

#include <common.h>
#include <ds/buffer.h>
#include <ds/map.h>
#include <ds/set.h>

typedef size_t symbol; /* As defined in the big comment below */
typedef size_t state;  /* Index for Grammar.collection */
#define map_new_symbol map_new_size_t
#define set_new_symbol set_new_size_t
#define map_new_state map_new_size_t
#define hash_state hash_size_t
#define equal_state equal_size_t
#define copy_state copy_size_t
#define destroy_state destroy_size_t
#define hash_symbol hash_size_t
#define equal_symbol equal_size_t
#define copy_symbol copy_size_t
#define destroy_symbol destroy_size_t

enum AssociationType { UNDEFINED_ASSOC, NONASSOC, LEFT_ASSOC, RIGHT_ASSOC };
typedef struct {
    size_t prio;  /* Priority, a < b means a comes before b */
    size_t assoc; /* One of the above */
} Precedence;

typedef struct {
    symbol lhs;
    buffer rhs; /* buffer<symbol> */
    symbol hint;
    size_t prefer;
} Production;

typedef void *(*sdt_callback)(void **sub);

/*
    Symbols are assumed to be terminals + N_TOKENS + non-terminals + N_SYMBOLS
        - sym = 0                 <==> Epsilon
        - 0 < sym < N_TOKENS      <==> Terminal (token)
        - sym = N_TOKENS          <==> Augmented start symbol
        - N_TOKENS < sym < N_SYM  <==> Non-terminal
        - sym = N_SYMBOLS         <==> $
*/
#define EPSILON 0
#define IS_TERMINAL(G, X) ((X) < ((G)->ntok))
#define IS_NON_TERMINAL(G, X) (!(IS_TERMINAL(X)))

typedef struct {
    buffer g; /* buffer<Production> */
    map prec; /* map<symbol, Precedence> */
    size_t ntok;
    size_t nsym;
    symbol start;
    bool augmented;
    map firsts;        /* map<symbol, set<symbol>> */
    map epsilons;      /* map<symbol, bool> */
    map closure_cache; /* map<Item, set<Item>> */
    buffer collection; /* buffer<set<Item>> */
    buffer gotos;      /* buffer<map<symbol, state>> */
    buffer table;      /* buffer<map<symbol, Entry>> */

    /* Debugging (showing errors) */
    const char **strtokens;
    const char **strnts;
} Grammar;

/* LR(1) item */
typedef struct {
    size_t prod; /* index for Grammar.g */
    size_t dot;  /* position before symbol in rhs */
    set look;    /* second component, lookahead symbols: set<symbol> */
} Item;
/* Value of "dot" when it's at the end of the production */
#define END_OF_PRODUCTION (~0ul)
Item Item_new(size_t prod, size_t dot, size_t nlook, ...);
size_t hash_item(const void *ptr);
size_t hash_item_core(const void *ptr); /* LALR */
size_t equal_item(const void *a, const void *b);
size_t equal_item_core(const void *a, const void *b); /* LALR */
void *copy_item(const void *a);
void destroy_item(void *a);
#define set_new_Item(S)                                                        \
    set_new(S, hash_item, equal_item, copy_item, destroy_item)
#define set_new_Item_core(S)                                                   \
    set_new(S, hash_item_core, equal_item_core, copy_item, destroy_item)

/* LR parsing table field */
enum EntryType { ENTRY_ACCEPT, ENTRY_REDUCE, ENTRY_SHIFT, ENTRY_GOTO };
typedef struct {
    size_t type; /* One of the above */
    size_t info; /* Additional information (state or production) */
} Entry;
size_t hash_entry(const void *ptr);
size_t equal_entry(const void *a, const void *b);
void *copy_entry(const void *a);
void destroy_entry(void *a);

/* Grammar.c */
void Grammar_new(Grammar *g, size_t ntok, size_t nsym, symbol start);
void Grammar_set_debugging(
    Grammar *g, const char **strtokens, const char **strnts);
void Grammar_add(Grammar *g, symbol lhs, buffer rhs);
void Grammar_add_hint(Grammar *g, size_t prod, symbol hint);
void Grammar_prefer(Grammar *g, size_t prod, size_t n);
void Grammar_shrink(Grammar *g);
void Grammar_clean(Grammar *g); /* Free everything not needed for parsing */
void Grammar_out(Grammar *g);

void Grammar_compile(Grammar *g);

/* Parser */
typedef struct {
    symbol sym;
    void *data; /* Anything you want! */
} StreamElement;

#ifndef IR_TYPEDEF
#define IR_TYPEDEF
/* We're going full data-oriented design for this one */
typedef struct {
    /* Types and data of children */
    buffer types; /* buffer<size_t (IRType)> */
    buffer ids;   /* buffer<size_t (iToken/iIR)> */
} IR;
typedef size_t iIR;
typedef buffer IRs; /* buffer<IR> */
#endif

IRs Grammar_parse(Grammar *g, const StreamElement *stream);

/* Serialization */
buffer Grammar_serialize(const Grammar *g);
Grammar *Grammar_deserialize(buffer in);

#endif
