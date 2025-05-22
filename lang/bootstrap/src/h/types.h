#ifndef TYPES_H
#define TYPES_H

#include <ds/buffer.h>

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
    TYPE_MODULE
} TypeId;

#define TYPE_FLAG_MUTABLE 0x1

typedef struct {
    TypeId id;
    void *data;
    size_t flags;
} Type;

typedef struct {
    buffer params;
    Type ret;
} Function;

typedef buffer Types; /* buffer<Type> */

#endif
