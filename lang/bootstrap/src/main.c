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
    SemResult sem;
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
    code = file_read_whole(argv[1]);

    /* Lexical analysis */
    tokens = get_tokens(code);
    free(code);

    /* Parse */
    irs = parse(tokens);

    /* Semantic analysis */
    sem = semantics(tokens, irs);

    /* Emit */
    emitted = emit(tokens, irs, sem);

    /* TODO: this leaks everywhere */
    buffer_out(&tokens);
    buffer_out(&irs);
    buffer_out(&sem.types);
    buffer_out(&sem.mangling);

    printf("\n\n--- EMITTED CODE ---\n");
    printf("%s", emitted);
    free(emitted);

    return 0;
}
