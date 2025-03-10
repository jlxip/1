#ifndef STRING_H
#define STRING_H

#include <stddef.h>

typedef struct {
    char *a;
    size_t len;
} string;

string snew(void);
string sc(const char *x);
string sdup(string x);
void sappend(string x, string y);
void sappendc(string x, const char *y);
const char *sget(string x);
size_t slen(string x);
void sfree(string x);

#endif
