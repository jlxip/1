#ifndef LEXER_H
#define LEXER_H

#include <common.h>
#include <tokens.h>

typedef struct {
    size_t consumed;
    Token token;
} Capture;

#define OK_TOKEN(X)                                                            \
    do {                                                                       \
        ret->consumed = cur - begin;                                           \
        ret->token.id = X;                                                     \
        return 1;                                                              \
    } while (0)

#define OK_TOKEN_DATA(X, T, Y)                                                 \
    do {                                                                       \
        ret->consumed = cur - begin;                                           \
        ret->token.id = X;                                                     \
        ret->token.data.T = Y;                                                 \
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
