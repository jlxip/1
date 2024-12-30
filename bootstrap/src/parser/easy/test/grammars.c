#include "grammars.h"
#include <stddef.h>

/*
    forfirst: non-tricky grammar
    Source: Dragon Book, Example 4.17 (Grammar 4.11)

    E  -> T E'
    E' -> + T E' | epsilon
    T  -> F T'
    T' -> * F T' | epsilon
    F  -> ( E ) | id
*/
const char *tokens_forfirst[6] = {"+", "*", "(", ")", "id", NULL};
const char *nts_forfirst[6] = {"E", "Ep", "T", "Tp", "F", NULL};
const char *grammar_forfirst = "E  -> T Ep\n"
                               "Ep -> + T Ep | EPSILON\n"
                               "T  -> F Tp\n"
                               "Tp -> * F Tp | EPSILON\n"
                               "F  -> ( E ) | id\n";

/*
    forfirst2: special case with epsilon #1
    Source: came up with it

    E -> A B
    A -> a | epsilon
    B -> b
*/
const char *tokens_forfirst2[3] = {"a", "b", NULL};
const char *nts_forfirst2[4] = {"E", "A", "B", NULL};
const char *grammar_forfirst2 = "E -> A B\n"
                                "A -> a | EPSILON\n"
                                "B -> b\n";

/*
    forfirst3: Special case with epsilon #2
    Source: came up with it

    E -> A B
    A -> a | epsilon
    B -> b | epsilon
*/

const char *tokens_forfirst3[3] = {"a", "b", NULL};
const char *nts_forfirst3[4] = {"E", "A", "B", NULL};
const char *grammar_forfirst3 = "E -> A B\n"
                                "A -> a | EPSILON\n"
                                "B -> b | EPSILON\n";

/*
    basic: the ONLY reference example for LALR
    Source: Dragon Book, Example 4.42

    (S' -> S)
    S -> CC
    C -> cC | d
*/

const char *tokens_basic[3] = {"c", "d", NULL};
const char *nts_basic[3] = {"S", "C", NULL};
const char *grammar_basic = "S -> C C\n"
                            "C -> c C | d\n";

/*
    lr: a textbook left and right example
    Source: random internet finding
        https://slideplayer.com/slide/13686742 - slides 21 and 22

    (S' -> S)
    S -> L=R | R
    L -> *R | id
    R -> L
*/
const char *tokens_lr[4] = {"=", "*", "id", NULL};
const char *nts_lr[4] = {"S", "L", "R", NULL};
const char *grammar_lr = "S -> L = R | R\n"
                         "L -> * R | id\n"
                         "R -> L\n";

/*
    random: just a random grammar I found
    Source: random internet finding + random LALR(1) Parser Generator
        https://jsmachines.sourceforge.net/machines/lalr1.html

    (S' -> S)
    S -> S + T | T
    T -> TF | F
    F -> F* | a | b
*/
const char *tokens_random[5] = {"+", "*", "a", "b", NULL};
const char *nts_random[4] = {"S", "T", "F", NULL};
const char *grammar_random = "S -> S + T | T\n"
                             "T -> T F | F\n"
                             "F -> F * | a | b\n";

/*
    wtf: slightly difficult grammar
    Source: came up with it
    Used: alexpizarroj/lalr1-table-generator

    (S' -> S)
    S -> AB  | C
    A -> aA  | EPSILON
    B -> Bb  | x
    C -> cCd | e
*/
const char *tokens_wtf[7] = {"a", "b", "c", "d", "e", "x", NULL};
const char *nts_wtf[5] = {"S", "A", "B", "C", NULL};
const char *grammar_wtf = "S -> A B | C\n"
                          "A -> a A | EPSILON\n"
                          "B -> B b | x\n"
                          "C -> c C d | e\n";

/*
    epsilon: a language based on silence
    Source: came up with it

    (S' -> S)
    S -> EPSILON
*/
const char *tokens_epsilon[1] = {NULL};
const char *nts_epsilon[2] = {"S", NULL};
const char *grammar_epsilon = "S -> EPSILON\n";
