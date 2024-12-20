#ifndef COMPILER_H
#define COMPILER_H

#include <ds/buffer.h>

typedef buffer Tokens;
Tokens get_tokens(const char *code);

void parse(const char *code);

#endif
