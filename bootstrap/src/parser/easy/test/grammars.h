#ifndef TEST_GRAMMARS_H
#define TEST_GRAMMARS_H

#define ADDG(X, Y, Z)                                                          \
    extern const char *tokens_##X[Y];                                          \
    extern const char *nts_##X[Z];                                             \
    extern const char *grammar_##X

ADDG(forfirst, 6, 6);
ADDG(forfirst2, 3, 4);
ADDG(forfirst3, 3, 4);
ADDG(basic, 3, 3);
ADDG(lr, 4, 4);
ADDG(random, 5, 4);
ADDG(epsilon, 1, 2);

#endif
