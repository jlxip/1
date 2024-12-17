#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>

/* Incredibly common stuff */

typedef unsigned char uint8_t;
typedef size_t bool;
enum _bool_values { false, true };

#define _throw_show(X)                                                         \
    printf("--------------------\n"                                            \
           "Internal error thrown!\n"                                          \
           "File: %s\n"                                                        \
           "Line: %d\n"                                                        \
           "%s\n"                                                              \
           "--------------------\n",                                           \
        __FILE__, __LINE__, X)

#ifdef DEBUG
/* Use valgrind to print the stack trace */
#define throw(X)                                                               \
    do {                                                                       \
        _throw_show(X);                                                        \
        *(short *)0 = 0;                                                       \
    } while (0)
#else
#define throw(X)                                                               \
    do {                                                                       \
        _throw_show(X);                                                        \
        exit(99);                                                              \
    } while (0)
#endif

/* I don't quite like C's standard assert() */
#define assert(X)                                                              \
    do {                                                                       \
        if (!(X))                                                              \
            throw("Assertion failed: " #X);                                    \
    } while (0)

#define todo() throw("TO DO!")

#define NOT_FOUND (~0ul)

#endif
