#ifndef TYPES_H
#define TYPES_H

#include <ds/buffer.h>
#include <ds/map.h>

typedef enum {
    TYPE_UNKNOWN, /* default type for anything */
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
    TYPE_STRUCT_INST,
    TYPE_SELF,
    TYPE_MODULE
} TypeId;

#define TYPE_FLAG_MUTABLE (1 << 0)
#define TYPE_FLAG_METHOD (1 << 1)
#define TYPE_FLAG_STATIC (1 << 2)

#define IS_MUTABLE(N) (TYPE(N)->flags & TYPE_FLAG_MUTABLE)
#define IS_METHOD(N) (TYPE(N)->flags & TYPE_FLAG_METHOD)
#define IS_STATIC(N) (TYPE(N)->flags & TYPE_FLAG_STATIC)

typedef struct {
    TypeId id;
    union {
        void *ptr;
        size_t word;
    } data;
    size_t flags;
} Type;

typedef struct {
    buffer params;
    Type ret;
} Function;

typedef struct {
    map fields;  /* map<const char*, Type> */
    map methods; /* map<const char*, iIR> */
} Struct;

typedef buffer Types; /* buffer<Type> */

#endif
