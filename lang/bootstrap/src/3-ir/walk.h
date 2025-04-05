#ifndef WALK_H
#define WALK_H

#include <common.h>
#include <compiler.h>
#include <ds/buffer.h>
#include <ds/map.h>
#include <string.h>

#define SUB_AST(N) (*buffer_get(ast->sub, N, AST *))
#define IS_NAME(X) (strcmp(ctx->names[ast->prod], X) == 0)

/* Useful for debugging */
#define PRINT_NAME                                                             \
    do {                                                                       \
        printf("name: %s\n", ctx->names[ast->prod]);                           \
    } while (0)

typedef enum {
    TYPE_NOTHING, /* only used in function returns */
    /* Atomic types */
    TYPE_BOOL,
    TYPE_BYTE,
    TYPE_FLOAT,
    TYPE_PTR,
    TYPE_STRING,
    TYPE_WORD,
    TYPE_TUPLE,
    /* Others */
    TYPE_FUNC,
    TYPE_STRUCT_DEF,
    TYPE_STRUCT_SPECIFIC,
    TYPE_STRUCT_INST
} TypeId;

typedef struct {
    TypeId id;
    void *data;
} Type;

typedef struct {
    TokenIdx name;
    Type type;
    bool mut;
    bool ref;
} Declaration;

typedef map SymbolTable; /* map<char*, Declaration> */
typedef buffer Symbols;  /* buffer<SymbolTable> */

typedef struct {
    Tokens tokens;
    const char **names; /* name of productions */
    Symbols syms;
} WalkCtx;

#define TOKEN(N) (buffer_get(ctx->tokens, N, Token))

#define NEW_SCOPE                                                              \
    do {                                                                       \
        map _x = NULL;                                                         \
        map_new_string(&_x, sizeof(Declaration), NULL, NULL, NULL, NULL);      \
        buffer_push(ctx->syms, &_x);                                           \
    } while (0)

#define TOP_SCOPE (*buffer_back(ctx->syms, SymbolTable))

#define PUSH_TO_SCOPE(NAME, DECL)                                              \
    do {                                                                       \
        SymbolTable st = TOP_SCOPE;                                            \
        map_remove_if_there(st, NAME);                                         \
        map_add(st, NAME, &DECL);                                              \
    } while (0)

#define POP_SCOPE                                                              \
    do {                                                                       \
        buffer_pop(ctx->syms);                                                 \
    } while (0)

#endif
