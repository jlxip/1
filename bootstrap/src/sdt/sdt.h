#ifndef SDT_H
#define SDT_H

#include <stdio.h>

#define DEFINE(X) void *sdt_##X(void **sub)

#define NOT_IMPLEMENTED                                                        \
    (void)sub;                                                                 \
    todo();                                                                    \
    return NULL

#endif
