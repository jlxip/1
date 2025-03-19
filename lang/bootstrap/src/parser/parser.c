#define GET_TOKEN_STRINGS

#include "easy/easy.h"
#include <compiler.h>
#include <files.h>
#include <stdio.h>
#include <stdlib.h>
#include <tokens.h>

const char *nts[] = {"S", "USES", "USE", "RELATIVE_PATH", "MODULE", "GLOBALS",
    "GLOBAL", "ANNOTATIONS", "ANNOTATION", "PRIMARY_LIST", "FUNCTION",
    "TYPED_IDS", "EXPRESSION", "LITERAL", "ASSIGNMENT", "PRIMARY",
    "DECLARATION", "TYPED_ID", "TYPE", "TYPES", "CONTROL", "CONDITIONAL", "IF",
    "ELIF", "ELSE", "LOOP", "FOR", "WHILE", "STRUCT", "STRUCT_DEF", "SWITCH",
    "SWITCH_BODY", "SWITCH_CASE", "BLOCK", "STATEMENTS", "STATEMENT", NULL};

ASTRoot parse(const Tokens tokens) {
    void *g;
    char *gtext;
    TokenData *stream;
    size_t i;
    ASTRoot ret;

    /* Compile grammar */
    gtext = read_whole_file("src/parser/grammar.txt");
    g = grammar(token_strings + 1, nts, gtext, "S");
    free(gtext);
    grammar_compile(g);

    /* Create stream of TokenData */
    stream = malloc((buffer_num(tokens) + 1) * sizeof(TokenData));
    for (i = 0; i < buffer_num(tokens); ++i) {
        /* data is Capture in this implementation */
        stream[i].sym = buffer_get(tokens, i, Capture)->token;
        stream[i].data = buffer_get(tokens, i, Capture);
    }
    stream[buffer_num(tokens)].sym = 0;
    stream[buffer_num(tokens)].data = NULL;

    /* Parse token */
    ret = grammar_parse(g, stream);

    /* Free */
    free(stream);
    grammar_out(g);

    return ret;
}
