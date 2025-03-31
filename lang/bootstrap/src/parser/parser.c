#define GET_TOKEN_STRINGS

#include "easy/easy.h"
#include <compiler.h>
#include <files.h>
#include <stdio.h>
#include <stdlib.h>
#include <tokens.h>

const char *nts[] = {"S", "USES", "USE", "RELATIVE_PATH", "MODULE", "GLOBALS",
    "GLOBAL", "ANNOTATIONS", "ANNOTATION", "PRIMARY_LIST", "FUNCTION", "PARAMS",
    "PARAM", "EXPRESSION", "EXPRESSION_LIST_OR_NONE", "EXPRESSION_LIST",
    "LITERAL", "TUPLE", "STRUCT_INST", "STRUCT_FIELD_INST", "ASSIGNMENT",
    "PRIMARY", "DECLARATION", "TYPED_ID", "TYPE", "TUPLEDEF", "TYPES",
    "CONTROL", "CONDITIONAL", "IF", "ELIF", "ELSE", "SWITCH", "SWITCH_BODY",
    "SWITCH_CASE", "LOOP", "FOR", "WHILE", "STRUCT", "STRUCT_DEF", "IMPL",
    "IMPL_DEF", "EXTERN", "EXTERN_ARGS", "EXTERN_ARG", "BLOCK", "STATEMENTS",
    "STATEMENT"};

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
