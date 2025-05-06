#include <ds/string.h>
#include <stdlib.h>
#include <string.h>

string snew(void) {
    string r;
    r.a = NULL;
    r.len = 0;
    return r;
}

string sc(const char *x) {
    string r;
    r.len = strlen(x);
    r.a = malloc(r.len + 1);
    memcpy(r.a, x, r.len + 1);
    return r;
}

string sdup(string x) {
    string r;
    r.len = x.len;
    r.a = malloc(r.len + 1);
    memcpy(r.a, x.a, r.len + 1);
    return r;
}

void sadd(string *x, string y) {
    size_t sz = x->len;
    x->len += y.len;
    x->a = realloc(x->a, x->len + 1);
    memcpy(x->a + sz, y.a, y.len + 1);
}

void saddc(string *x, const char *y) {
    size_t sz = x->len;
    size_t os = strlen(y);
    x->len += os;
    x->a = realloc(x->a, x->len + 1);
    memcpy(x->a + sz, y, os + 1);
}

/* MAYBE: CRLF vs LF? */
void snewln(string *x) { saddc(x, "\n"); }

void saddln(string *x, string y) {
    sadd(x, y);
    snewln(x);
}

void saddlnc(string *x, const char *y) {
    saddc(x, y);
    snewln(x);
}

char *sget(string x) { return x.a; }

size_t slen(string x) { return x.len; }

void sfree(string *x) {
    free(x->a);
    x->a = NULL;
    x->len = 0;
}
