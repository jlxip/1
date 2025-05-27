#include "files.h"
#include <stdio.h>
#include <string.h>

#define CHUNK_SIZE 1024

bool file_exists(const char *path) {
    FILE *file;

    file = fopen(path, "rb");
    if (file == NULL)
        return false;
    fclose(file);
    return true;
}

buffer file_read_bytes(const char *path) {
    buffer ret = NULL;
    FILE *file;
    uint8_t byte;

    file = fopen(path, "rb");
    if (!file)
        throwe("Could not open file: %s\n", path);

    buffer_new(&ret, sizeof(uint8_t));
    while (fread(&byte, 1, 1, file))
        buffer_push(ret, &byte);

    fclose(file);
    return ret;
}

void file_write_bytes(const char *path, buffer buf) {
    FILE *file;
    size_t i;

    file = fopen(path, "wb");
    if (!file)
        throwe("Could not open file: %s\n", path);

    for (i = 0; i < buffer_num(buf); ++i)
        fwrite(buffer_get(buf, i, uint8_t), 1, 1, file);

    fclose(file);
}

char *file_read_whole(const char *path) {
    FILE *file;
    size_t size;
    char *ret, *cur;

    file = fopen(path, "r");
    if (!file)
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

void file_write_whole(const char *path, const char *txt) {
    FILE *file;

    file = fopen(path, "w");
    if (!file)
        throwe("Could not open file: %s\n", path);
    fwrite(txt, 1, strlen(txt), file);
    fclose(file);
}
