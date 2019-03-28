#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/lists/dllist.h"

typedef struct { int value; } Integer;

int compare_int (void *one, void *two) {

    if (one && two) return ((Integer *) one)->value <= ((Integer *) two)->value ? 0 : 1;

}

int main (void) {

    srand ((unsigned) time (NULL));

    DoubleList *list = dlist_init (NULL, compare_int);

    for (int i = 0; i < 100; i++) {
        Integer *integer = (Integer *) malloc (sizeof (int));
        integer->value = rand () % 99 + 1;
        dlist_insert_after (list, LIST_START (list), integer);
    }

    dlist_sort (list);
    for (ListElement *le = LIST_START (list); le != NULL; le = le->next) {
        Integer *integer = (Integer *) le->data;
        printf ("%3i", integer->value);
    }
    
    dlist_destroy (list);

    return 0;

}