#include <compiler.h>
#include <files.h>
#include <stdio.h>
#include <stdlib.h>
#include <tokens.h>

void test_lalr(void);

int main(int argc, const char *argv[]) {
    char *code;
    Tokens tokens;
    IRs irs;
    Types types;
    char *emitted;

#ifdef DEBUG
    printf("Testing LALR implementation\n");
    test_lalr();
#endif

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

    /* Parse */
    irs = parse(tokens);

    /* Semantic analysis */
    types = semantics(tokens, irs);

    /* Emit */
    emitted = emit(tokens, irs, types);

    buffer_out(&tokens);
    buffer_out(&irs);
    buffer_out(&types);

    printf("\n\n--- EMITTED CODE ---\n");
    printf("%s", emitted);

    return 0;
}
