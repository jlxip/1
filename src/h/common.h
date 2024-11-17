#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>

/* Incredibly common stuff */

#define throw(X)                                                               \
    do {                                                                       \
        printf("--------------------\n"                                        \
               "Internal error thrown!\n"                                      \
               "File: %s\n"                                                    \
               "Line: %d\n"                                                    \
               "%s\n"                                                          \
               "--------------------\n",                                       \
            __FILE__, __LINE__, X);                                            \
        exit(99);                                                              \
    } while (0)

/* I don't quite like C's standard assert() */
#define assert(X)                                                              \
    do {                                                                       \
        if (!(X))                                                              \
            throw("Assertion failed: " #X);                                    \
    } while (0)

#define todo() throw("TO DO!")

#define pass                                                                   \
    do {                                                                       \
    } while (0)

#endif
