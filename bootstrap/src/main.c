#include <compiler.h>
#include <files.h>
#include <stdio.h>
#include <stdlib.h>

void test_lalr(void);

int main(int argc, const char *argv[]) {
    char *code = NULL;

    printf("Testing LALR implementation\n");
    test_lalr();

    if (argc < 2) {
        printf("Usage:   1 <path/to/main>\n"
               "Example: 1 src/main.1\n");
        exit(99);
    }

    code = read_whole_file(argv[1]);
    parse(code);
    free(code);
    return 0;
}
