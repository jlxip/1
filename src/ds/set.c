#include <common.h>
#include <ds/set.h>

/*
    A really terrible implementation of unordered_set
    It is not worth to put much effort here since this is the stage0 compiler
*/

void set_new(set *s, size_t datasize) { buffer_new(s, datasize); }

void set_add(set s, const void *element) {
    if (set_has(s, element))
        return;
    buffer_push(s, element);
}

void set_remove(set s, const void *element) {
    size_t idx = buffer_find(s, element);
    assert(idx != BUFFER_NOT_FOUND);
    buffer_remove(s, idx);
}

void set_out(set *s) { buffer_out(s); }
void set_shrink(set s) { buffer_shrink(s); }

/* --- Getters --- */
size_t set_num(set s) { return buffer_num(s); }
size_t set_empty(set s) { return buffer_empty(s); }

size_t set_has(set s, const void *element) {
    size_t idx = buffer_find(s, element);
    return idx != BUFFER_NOT_FOUND;
}
