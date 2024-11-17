#include <compiler.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[]) {
    FILE *file = NULL;
    long sz = 0;
    char *code = NULL;
    if (argc < 2) {
        printf("Usage:   1 <path/to/main>\n"
               "Example: 1 src/main.1\n");
        exit(99);
    }

    file = fopen(argv[1], "r");
    if (!file) {
        printf("Could not open file: %s\n", argv[1]);
        exit(99);
    }

    fseek(file, 0, SEEK_END);
    sz = ftell(file);
    rewind(file);
    code = (char *)malloc(sz);
    fread(code, sz, 1, file);
    fclose(file);

    parse(code);
    free(code);
    return 0;
}
