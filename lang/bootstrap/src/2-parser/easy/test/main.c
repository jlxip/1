#include "grammars.h"
#include <stdio.h>

void test_lalr_easy(void);
void test_lalr_first(void);
void test_lalr_closure(void);
void test_lalr_goto(void);
void test_lalr_collection(void);
void test_lalr_table(void);
void test_lalr_edge(void);

#define test(TXT)                                                              \
    do {                                                                       \
        printf("-> " #TXT " ");                                                \
        fflush(0);                                                             \
        test_lalr_##TXT();                                                     \
        printf("[OK]\n");                                                      \
    } while (0)

void test_lalr(void) {
    test(easy);
    test(first);
    test(closure);
    test(goto);
    test(collection);
    test(table);
    test(edge);
}
