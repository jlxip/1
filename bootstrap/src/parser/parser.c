#define GET_TOKEN_STRINGS

#include "parser.h"
#include "easy/easy.h"
#include <files.h>
#include <stdlib.h>
#include <tokens.h>

const char *nts[] = {"S", "USES", "USE", "RELATIVE_PATH", "MODULE", "GLOBALS",
    "GLOBAL", "GLOBAL_DECLARATION", "MAYBE_TYPED_ID", "TYPE", "TYPES",
    "FUNCTION", "TYPED_IDS", "EXPRESSION", "ASSIGNMENT", "LHS", "RHS",
    "CONTROL", "CONDITIONAL", "LOOP", "BLOCK", "STATEMENTS", "STATEMENT", NULL};

void parse(buffer input) {
    void *g;
    char *gtext;

    gtext = read_whole_file("src/parser/grammar.txt");
    g = grammar(token_strings + 1, nts, gtext, "S");
    grammar_compile(g);
    grammar_out(g);

    (void)g;
    (void)input;

    free(gtext);
}
