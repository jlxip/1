#ifndef EASY_H
#define EASY_H

#include <stddef.h>

void *grammar(
    const char **tokens, const char **nts, const char *cstr, const char *start);
void grammar_compile(void *g);
void grammar_out(void *g);

typedef struct {
    size_t sym;
    void *data;
} TokenData;
void grammar_parse(void *g, const TokenData *stream);

#endif
