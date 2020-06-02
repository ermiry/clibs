#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/collections/htab.h"

typedef struct Data {

    int idx;
    int value;

} Data;

static inline void data_delete (void *data_ptr) {

    if (data_ptr) free (data_ptr);

}

static inline void data_print (Data *data) {

    if (data) printf ("idx: %d - value: %d\n", data->idx, data->value);

}

int main (void) {

    srand ((unsigned) time (NULL));

    Htab *map = htab_create (
        HTAB_DEFAULT_INIT_SIZE,
        NULL,
        NULL,
        NULL
    );

    int value = 1000;
    for (unsigned int i = 0; i < 100; i++) {
        const void *key = &i;

        Data *data = (Data *) malloc (sizeof (Data));
        data->idx = i;
        // data->value = rand () % 99 + 1;
        data->value = value;
        value++;

        htab_insert (
            map, 
            key, sizeof (int), 
            data, sizeof (Data)
        );
    }

    htab_print (map);

    for (unsigned int i = 0; i < 50; i++) {
        const void *key = &i;

        Data *data = (Data *) htab_remove (map, key, sizeof (int));
        if (data) {
            data_print (data);
            data_delete (data);
        }

        else {
            printf ("\n\nNO DATA FOR KEY: %d\n\n", i);
        }
    }

    printf ("\nDone!\n");

    return 0;

}