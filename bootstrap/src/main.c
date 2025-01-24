#include <compiler.h>
#include <files.h>
#include <stdio.h>
#include <stdlib.h>
#include <tokens.h>

void test_lalr(void);

int main(int argc, const char *argv[]) {
    char *code = NULL;
    Tokens tokens;
    size_t *stream;
    size_t i;

    printf("Testing LALR implementation\n");
    test_lalr();

    if (argc < 2) {
        printf("Usage:   1 <path/to/main>\n"
               "Example: 1 src/main.1\n");
        exit(99);
    }

    /* Read input file */
    code = read_whole_file(argv[1]);

    /* Lexical analysis */
    tokens = get_tokens(code);
    free(code);

    /* Create stream of tokens */
    stream = malloc((buffer_num(tokens) + 1) * sizeof(size_t));
    for (i = 0; i < buffer_num(tokens); ++i)
        stream[i] = buffer_get(tokens, i, Capture)->token;
    stream[buffer_num(tokens)] = 0;

    /* Parse */
    parse(stream, tokens);

    /* Free */
    buffer_out(&tokens);
    return 0;
}
