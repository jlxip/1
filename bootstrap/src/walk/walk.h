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

typedef enum {
    TYPE_BOOL,
    TYPE_WORD,
    TYPE_FLOAT,
    TYPE_STRING,
    /*TYPE_LIST,*/
    /*TYPE_DICT,*/
    TYPE_FUNC
    /*TYPE_STRUCT,*/
} Type;

typedef struct {
    size_t lineno;
    Type type;
    bool mut;
} Declaration;

size_t hash_declaration(const void *x);
size_t equal_declaration(const void *a, const void *b);
void *copy_declaration(const void *x);
void destroy_declaration(void *x);

typedef map SymbolTable;    /* map<char*, Declaration> */
typedef buffer Symbols;     /* buffer<SymbolTable> */

#define NEW_SCOPE                                                              \
    do {                                                                       \
        map _x = NULL;                                                         \
        map_new_string(&_x, sizeof(Declaration), hash_declaration,             \
            equal_declaration, copy_declaration, destroy_declaration);         \
        buffer_push(*syms, &_x);                                               \
    } while (0)

#define PUSH_TO_SCOPE(NAME, DECL)                                              \
    do {                                                                       \
        SymbolTable st = *buffer_back(*syms, SymbolTable);                     \
        map_add(st, NAME, &DECL);                                              \
    } while (0)

#define LOOKUP(X, NAME)                                                        \
    do {                                                                       \
        bool found = false;                                                    \
        size_t _nsyms = buffer_num(*syms);                                     \
        size_t _i;                                                             \
        for (_i = 0; _i < _nsyms; ++_i) {                                      \
            SymbolTable st = *buffer_get(*syms, _nsyms - _i - 1, SymbolTable); \
            if (map_has(st, NAME)) {                                           \
                X = map_get(st, NAME, Declaration);                            \
                found = true;                                                  \
                break;                                                         \
            }                                                                  \
        }                                                                      \
        if (!found)                                                            \
            throwe("undeclared symbol: %s", NAME);                             \
    } while (0)

#endif
