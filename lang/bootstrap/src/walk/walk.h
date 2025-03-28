#ifndef WALK_H
#define WALK_H

#include <common.h>
#include <compiler.h>
#include <ds/buffer.h>
#include <ds/map.h>
#include <string.h>

#define WALKFUNC(X) void walk_##X(AST *ast, const char **names, Symbols *syms)

#define SUB_AST(N) (*buffer_get(ast->sub, N, AST *))
#define IS_NAME(X) (strcmp(names[ast->prod], X) == 0)

/* Useful for debugging */
#define PRINT_NAME                                                             \
    do {                                                                       \
        printf("name: %s\n", names[ast->prod]);                                \
    } while (0)

typedef enum {
    /* Atomic types */
    TYPE_BOOL,
    TYPE_FLOAT,
    TYPE_PTR,
    TYPE_STRING,
    TYPE_WORD,
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
    size_t lineno;
    const char *name;
    Type type;
    bool mut;
} Declaration;

typedef map SymbolTable; /* map<char*, Declaration> */
typedef buffer Symbols;  /* buffer<SymbolTable> */

#define NEW_SCOPE                                                              \
    do {                                                                       \
        map _x = NULL;                                                         \
        map_new_string(&_x, sizeof(Declaration), NULL, NULL, NULL, NULL);      \
        buffer_push(*syms, &_x);                                               \
    } while (0)

#define TOP_SCOPE (*buffer_back(*syms, SymbolTable))

#define PUSH_TO_SCOPE(NAME, DECL)                                              \
    do {                                                                       \
        SymbolTable st = TOP_SCOPE;                                            \
        map_remove_if_there(st, NAME);                                         \
        map_add(st, NAME, &DECL);                                              \
    } while (0)

#define POP_SCOPE                                                              \
    do {                                                                       \
        buffer_pop(*syms);                                                     \
    } while (0)

#endif
