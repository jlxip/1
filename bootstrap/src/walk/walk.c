#include <compiler.h>
#include <stdio.h>

void walk(ASTRoot ast) {
    AST *first, *second, *aux;
    printf("prod: %s\n", ast.names[ast.ast.prod]);
    printf("nsub: %lu\n", buffer_num(ast.ast.sub));

    first = *buffer_get(ast.ast.sub, 0, AST *);
    second = *buffer_get(ast.ast.sub, 1, AST *);
    printf("first: %s\n", ast.names[first->prod]);
    printf("second: %s\n", ast.names[second->prod]);

    aux = second;
    first = *buffer_get(aux->sub, 0, AST *);
    second = *buffer_get(aux->sub, 1, AST *);
    printf("first: %s\n", ast.names[first->prod]);
    printf("second: %s\n", ast.names[second->prod]);

    printf("OK.\n");
    while (1)
        ;
}
