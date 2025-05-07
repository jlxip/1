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
void sadd(string *x, string y);
void saddc(string *x, const char *y);
void saddi(string *x, size_t y);

void snewln(string *x);
void saddln(string *x, string y);
void saddlnc(string *x, const char *y);

char *sget(string x);
size_t slen(string x);
void sfree(string *x);

#endif
