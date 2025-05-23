#ifndef FILES_H
#define FILES_H

#include <common.h>
#include <ds/buffer.h>

bool file_exists(const char *path);
buffer file_read_bytes(const char *path);
void file_write_bytes(const char *path, buffer buf);
char *file_read_whole(const char *path);

#endif
