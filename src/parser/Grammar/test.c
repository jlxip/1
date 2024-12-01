#include "Grammar.h"
#include <stdio.h>

enum _TEST_LALR_FF {
    FF_T_NULL,
    FF_T_plus,
    FF_T_star,
    FF_T_opar,
    FF_T_cpar,
    FF_T_id,
    FF_T_NTOKENS,

    FF_NT_E,
    FF_NT_Ep,
    FF_NT_T,
    FF_NT_Tp,
    FF_NT_F
};

void test_lalr(void) {
    /*
        First test FIRST and FOLLOW
        Source: Dragon Book, Example 4.17

        E  -> T E'
        E' -> + T E' | epsilon
        T  -> F T'
        T' -> * F T' | epsilon
        F  -> ( E ) | id
    */
    Grammar g;
    buffer buf; /* recycled */
    size_t x;   /* recycled */

    Grammar_new(&g, FF_T_NTOKENS, FF_NT_E);
    Grammar_add(&g, FF_NT_E, 2, FF_NT_T, FF_NT_Ep);
    Grammar_add(&g, FF_NT_Ep, 3, FF_T_plus, FF_NT_T, FF_NT_Ep);
    Grammar_add(&g, FF_NT_Ep, EPSILON);
    Grammar_add(&g, FF_NT_T, 2, FF_NT_F, FF_NT_Tp);
    Grammar_add(&g, FF_NT_Tp, 3, FF_T_star, FF_NT_F, FF_NT_Tp);
    Grammar_add(&g, FF_NT_Tp, EPSILON);
    Grammar_add(&g, FF_NT_F, 3, FF_T_opar, FF_NT_E, FF_T_cpar);
    Grammar_add(&g, FF_NT_F, 1, FF_T_id);
    Grammar_augment(&g);
    Grammar_shrink(&g);

    /* FIRST(E) = { (, id } */
    buf = get_first(&g, FF_NT_E);
    assert(buffer_num(buf) == 2);
    x = FF_T_opar;
    assert(buffer_has(buf, &x));
    x = FF_T_id;
    assert(buffer_has(buf, &x));
    buffer_out(&buf);
    /* FIRST(E') = { +, epsilon } */
    buf = get_first(&g, FF_NT_Ep);
    assert(buffer_num(buf) == 2);
    x = FF_T_plus;
    assert(buffer_has(buf, &x));
    x = EPSILON;
    assert(buffer_has(buf, &x));
    buffer_out(&buf);
    /* FIRST(T) = { (, id } */
    buf = get_first(&g, FF_NT_T);
    assert(buffer_num(buf) == 2);
    x = FF_T_opar;
    assert(buffer_has(buf, &x));
    x = FF_T_id;
    assert(buffer_has(buf, &x));
    buffer_out(&buf);
    /* FIRST(T') = { *, epsilon } */
    buf = get_first(&g, FF_NT_Tp);
    assert(buffer_num(buf) == 2);
    x = FF_T_star;
    assert(buffer_has(buf, &x));
    x = EPSILON;
    assert(buffer_has(buf, &x));
    buffer_out(&buf);
    /* FIRST(F) = { (, id } */
    buf = get_first(&g, FF_NT_F);
    assert(buffer_num(buf) == 2);
    x = FF_T_opar;
    assert(buffer_has(buf, &x));
    x = FF_T_id;
    assert(buffer_has(buf, &x));
    buffer_out(&buf);

    printf("NICE!\n");
    Grammar_out(&g);
}
