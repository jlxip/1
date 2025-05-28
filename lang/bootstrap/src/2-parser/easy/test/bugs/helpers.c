#include "helpers.h"

void print_item(Item *item) {
    set_iterator it;

    printf("ITEM:\n");
    printf("  prod: %lu\n", item->prod);
    printf("  dot:  %lu\n", item->dot);
    printf("  look: ");
    it = set_it_begin(item->look);
    while (!set_it_finished(&it)) {
        printf("%lu, ", *set_it_get(&it, symbol));
        set_it_next(&it);
    }
    printf("\n");
}

void print_set_of_items(set s) {
    set_iterator it;
    it = set_it_begin(s);
    while (!set_it_finished(&it)) {
        print_item(set_it_get(&it, Item));
        set_it_next(&it);
    }
}
