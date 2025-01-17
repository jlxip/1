#include "files.h"
#include <common.h>
#include <stdio.h>

#define CHUNK_SIZE 1024

char *read_whole_file(const char *path) {
    FILE *file;
    size_t size;
    char *ret, *cur;

    file = fopen(path, "r");
    if (file == NULL)
        throwe("Could not open file: %s\n", path);

    size = CHUNK_SIZE;
    cur = ret = (char *)malloc(size);
    for (;;) {
        size_t read = fread(cur, 1, CHUNK_SIZE, file);
        if (read == CHUNK_SIZE) {
            /* Need more */
            ret = realloc(ret, size + CHUNK_SIZE);
            cur = ret + size;
            size += CHUNK_SIZE;
        } else {
            /* Finished! */
            cur[read] = '\0';
            break;
        }
    }

    fclose(file);
    return ret;
}
