#ifndef WALK_H
#define WALK_H

#include <ds/buffer.h>
#include <ds/map.h>

#define WALKFUNC(X) void walk_##X(AST *ast, const char **names, Symbols *syms)

#define SUB_AST(N) (*buffer_get(ast->sub, N, AST *))
#define IS_NAME(X) (strcmp(names[ast->prod], X) == 0)

typedef struct {
    size_t lineno;
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
        buffer_push(syms, &_x);                                                \
    } while (0)

#endif
