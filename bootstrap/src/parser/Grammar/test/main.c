void test_lalr_easy(void);
void test_lalr_first(void);
void test_lalr_closure(void);
void test_lalr_goto(void);
void test_lalr_collection(void);
void test_lalr_table(void);

void test_lalr(void) {
    test_lalr_easy();
    test_lalr_first();
    test_lalr_closure();
    test_lalr_goto();
    test_lalr_collection();
    test_lalr_table();
}
