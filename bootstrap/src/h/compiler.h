#ifndef COMPILER_H
#define COMPILER_H

#include <ds/buffer.h>

typedef buffer Tokens; /* buffer<Capture> */
Tokens get_tokens(const char *code);
void parse(const Tokens tokens);
void setup_sdt(void *ptr);

#endif
