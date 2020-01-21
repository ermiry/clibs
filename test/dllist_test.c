#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/lists/dllist.h"

typedef struct { int value; } Integer;

int compare_int (const void *one, const void *two) {

    if (one && two) return ((Integer *) one)->value <= ((Integer *) two)->value ? 0 : 1;

}

static void test_sort (void) {

    DoubleList *list = dlist_init (NULL, compare_int);

    for (int i = 0; i < 100; i++) {
        Integer *integer = (Integer *) malloc (sizeof (int));
        integer->value = rand () % 99 + 1;
        dlist_insert_after (list, dlist_start (list), integer);
    }

    dlist_sort (list);
    for (ListElement *le = dlist_start (list); le != NULL; le = le->next) {
        Integer *integer = (Integer *) le->data;
        printf ("%3i", integer->value);
    }
    
    dlist_delete (list);

}

// uncomment the function that represents the test you want to run and the follow these steps
// from test directory...
// mkdir bin
// to compile run: gcc ./test/dllist_test.c ./src/lists/dllist.c -o ./bin/dllist_test
// and run using: ./bin/dllist_test
int main (void) {

    srand ((unsigned) time (NULL));

    test_sort ();

    return 0;

}