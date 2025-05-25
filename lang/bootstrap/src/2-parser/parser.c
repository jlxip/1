#define GET_TOKEN_STRINGS

#include "easy/easy.h"
#include <compiler.h>
#include <files.h>
#include <stdio.h>
#include <stdlib.h>
#include <tokens.h>

const char GRAMMAR_PATH[] = "src/2-parser/grammar.txt";
const char SERIALIZED_PATH[] = "src/2-parser/grammar.bin";

const char *nts[] = {"S", "USES", "USE", "GLOBALS", "GLOBAL", "ANNOTATIONS",
    "ANNOTATION", "FUNCTION", "PARAMS", "PARAM", "EXPRESSION", "NAME",
    "EXPRESSION_LIST", "LITERAL", "TUPLE", "STRUCT_INST", "STRUCT_FIELD_INST",
    "ASSIGNMENT", "DECLARATION", "TYPED_ID", "TYPE", "PATH", "TUPLEDEF",
    "TYPES", "CONTROL", "CONDITIONAL", "IF", "ELIF", "ELSE", "SWITCH",
    "SWITCH_BODY", "SWITCH_CASE", "LOOP", "FOR", "WHILE", "STRUCT",
    "STRUCT_DEF", "IMPL", "IMPL_DEF", "EXTERN", "EXTERN_ARGS", "EXTERN_ARG",
    "BLOCK", "STATEMENTS", "STATEMENT", NULL};

void *get_grammar(void) {
    void *ret;
    char *gtext;

#ifndef NO_GRAMMAR_CACHE
    if (file_exists(SERIALIZED_PATH))
        return grammar_load(
            file_read_bytes(SERIALIZED_PATH), token_strings + 1, nts);
#endif

    gtext = file_read_whole(GRAMMAR_PATH);
    ret = grammar(token_strings + 1, nts, gtext, "S");
    free(gtext);
    grammar_compile(ret);

#ifndef NO_GRAMMAR_CACHE
    file_write_bytes(SERIALIZED_PATH, grammar_save(ret));
#endif
    return ret;
}

/* --- */

IRs parse(Tokens tokens) {
    void *g;
    TokenData *stream;
    size_t i;
    IRs ret;

    g = get_grammar();

    /* Create stream of TokenData */
    stream = malloc((buffer_num(tokens) + 1) * sizeof(TokenData));
    for (i = 0; i < buffer_num(tokens); ++i) {
        /* data is iToken in this implementation */
        stream[i].sym = buffer_get(tokens, i, Token)->id;
        stream[i].data = i;
    }
    stream[buffer_num(tokens)].sym = 0;
    stream[buffer_num(tokens)].data = 0;

    /* Parse token */
    ret = grammar_parse(g, stream);

    /* Free */
    free(stream);
    grammar_out(g);

    return ret;
}
