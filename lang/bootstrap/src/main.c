#include <compiler.h>
#include <files.h>
#include <stdio.h>
#include <stdlib.h>
#include <tokens.h>

void test_lalr(void);

int main(int argc, const char *argv[]) {
    char *code = NULL;
    Tokens tokens;
    ASTPack ast;

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
    ast = parse(tokens);

    /* Walk the AST */
    walk(ast, tokens);

    buffer_out(&tokens);
    return 0;
}
