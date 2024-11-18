#ifndef LEXER_H
#define LEXER_H

#include <common.h>
#include <tokens.h>

/* This applies for all switches in this folder */
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif

#define OK_TOKEN(X)                                                            \
    do {                                                                       \
        ret->ok = 1;                                                           \
        ret->consumed = cur - begin;                                           \
        ret->token = X;                                                        \
        return 1;                                                              \
    } while (0)

#define OK_TOKEN_INFO(X, Y)                                                    \
    do {                                                                       \
        ret->ok = 1;                                                           \
        ret->consumed = cur - begin;                                           \
        ret->token = X;                                                        \
        ret->info = Y;                                                         \
        return 1;                                                              \
    } while (0)

/* Regex-like matches */
#define IS_AZ(X) ((X) >= 'A' && (X) <= 'Z')
#define IS_az(X) ((X) >= 'a' && (X) <= 'z')
#define IS_09(X) ((X) >= '0' && (X) <= '9')
#define IS__(X) ((X) == '_')

/* For internal use of the lexer */
size_t match_keyword(Capture *ret, const char *cur);
size_t match_id(Capture *ret, const char *cur);
size_t match_symbol(Capture *ret, const char *cur);
size_t match_numeric(Capture *ret, const char *cur);
size_t match_string(Capture *ret, const char *cur);
buffer get_tokens(const char *code);

#endif
