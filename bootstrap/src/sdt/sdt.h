#ifndef SDT_H
#define SDT_H

#include <stdio.h>

/* MODULE */
void *sdt_relpath_dot(void **sub, size_t nsub);
void *sdt_relpath_direct(void **sub, size_t nsub);
void *sdt_module_dot(void **sub, size_t nsub);
void *sdt_module_direct(void **sub, size_t nsub);

#endif
