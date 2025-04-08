#include <common.h>
#include <compiler.h>

void modules_main(IRs irs, Tokens tokens) {
    IR *program;

    (void)irs;
    (void)tokens;

    printf("IRs: %lu\n", buffer_num(irs));
    program = buffer_back(irs, IR);
    assert(buffer_num(program->types) == 2);
    printf("Type of first child: %lu\n", *buffer_front(program->types, size_t));
    printf("Type of second child: %lu\n", *buffer_back(program->types, size_t));
}
