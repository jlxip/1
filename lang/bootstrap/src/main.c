#include <compiler.h>
#include <files.h>
#include <stdio.h>
#include <stdlib.h>
#include <tokens.h>

void test_lalr(void);

int main(int argc, const char *argv[]) {
    char *code = NULL;
    Tokens tokens;
    IRs irs;

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

    /* Run modules */
    modules_main(irs, tokens);

    buffer_out(&tokens);
    printf("That's all for now folks!\n");
    return 0;
}
