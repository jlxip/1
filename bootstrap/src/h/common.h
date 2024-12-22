#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>

/* Incredibly common stuff */

typedef unsigned char uint8_t;
typedef size_t bool;
enum _bool_values { false, true };

#define _throw_show_header                                                     \
    printf("--------------------\n"                                            \
           "Internal error thrown!\n"                                          \
           "File: %s\n"                                                        \
           "Line: %d\n",                                                       \
        __FILE__, __LINE__)

#define _throw_show_footer                                                     \
    printf("\n"                                                                \
           "--------------------\n")

#ifdef DEBUG
/* Use valgrind to print the stack trace */
#define throw(X)                                                               \
    do {                                                                       \
        _throw_show_header;                                                    \
        printf("%s", X);                                                       \
        _throw_show_footer;                                                    \
        *(short *)0 = 0;                                                       \
    } while (0)
#define throwe(X, Y)                                                           \
    do {                                                                       \
        _throw_show_header;                                                    \
        printf(X, Y);                                                          \
        _throw_show_footer;                                                    \
        *(short *)0 = 0;                                                       \
    } while (0)
#else
#define throw(X)                                                               \
    do {                                                                       \
        _throw_show_header;                                                    \
        printf("%s", X);                                                       \
        _throw_show_footer;                                                    \
        exit(99);                                                              \
    } while (0)
#define throwe(X, Y)                                                           \
    do {                                                                       \
        _throw_show_header;                                                    \
        printf(X, Y);                                                          \
        _throw_show_footer;                                                    \
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
