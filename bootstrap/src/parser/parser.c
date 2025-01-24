#define GET_TOKEN_STRINGS

#include "easy/easy.h"
#include <compiler.h>
#include <files.h>
#include <stdio.h>
#include <stdlib.h>
#include <tokens.h>

const char *nts[] = {"S", "USES", "USE", "RELATIVE_PATH", "MODULE", "GLOBALS",
    "GLOBAL", "FUNCTION", "TYPED_IDS", "EXPRESSION", "ASSIGNMENT", "LHS",
    "TYPED_ID", "TYPE", "TYPES", "RHS", "BLOCK", "STATEMENTS", "STATEMENT",
    NULL};

void parse(const size_t *tokens, void *data) {
    void *g;
    char *gtext;

    /* Compile grammar */
    gtext = read_whole_file("src/parser/grammar.txt");
    g = grammar(token_strings + 1, nts, gtext, "S");
    free(gtext);
    grammar_compile(g);

    /* Parse token */
    grammar_parse(g, tokens, data);
    printf("Accepted!\n");

    grammar_out(g);
}
