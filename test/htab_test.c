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

int test_int_insert (void) {

    printf ("\ntest_int_insert\n");

    Htab *map = htab_create (
        HTAB_DEFAULT_INIT_SIZE,
        NULL,
        NULL,
        data_delete
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

    htab_destroy (map);

    return 0;

}

int test_int_remove (void) {

    printf ("\ntest_int_remove\n");

    int n_elements = 100;

    Htab *map = htab_create (
        HTAB_DEFAULT_INIT_SIZE,
        NULL,
        NULL,
        data_delete
    );

    int value = 1000;
    for (unsigned int i = 0; i < n_elements; i++) {
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

    printf ("\nItems in map after insert: %ld / %d\n", map->count, n_elements);

    for (unsigned int i = 0; i < n_elements; i++) {
        const void *key = &i;

        Data *data = (Data *) htab_remove (map, key, sizeof (int));
        if (data) {
            // data_print (data);
            data_delete (data);
        }

        else {
            printf ("\n\nNO DATA FOR KEY: %d\n\n", i);
        }
    }

    printf ("\nItems in map after remove: %ld / 0\n", map->count);

    htab_destroy (map);

    return 0;

}

int test_int_get (void) {

    printf ("\ntest_int_get\n");

    int n_elements = 100;

    Htab *map = htab_create (
        HTAB_DEFAULT_INIT_SIZE,
        NULL,
        NULL,
        data_delete
    );

    int value = 1000;
    for (unsigned int i = 0; i < n_elements; i++) {
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

    printf ("\nItems in map after insert: %ld / %d\n", map->count, n_elements);

     for (unsigned int i = 0; i < n_elements; i++) {
        // const void *key = &i;

        int key_int = rand () % ((n_elements * 2) - 1) + 1;;
        const void *key = &key_int;

        Data *data = (Data *) htab_get (map, key, sizeof (int));
        if (data) {
            data_print (data);
        }

        else {
            if (key_int > n_elements) printf ("[OKAY]: no data for key: %d\n", key_int);
            else printf ("[ERROR]: NO DATA FOR KEY: %d\n", key_int);
        }
    }

    htab_destroy (map);

    return 0;

}

int test_int_contains (void) {

    printf ("\ntest_int_contains\n");

    int n_elements = 100;

    Htab *map = htab_create (
        HTAB_DEFAULT_INIT_SIZE,
        NULL,
        NULL,
        data_delete
    );

    int value = 1000;
    for (unsigned int i = 0; i < n_elements; i++) {
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

    printf ("\nItems in map after insert: %ld / %d\n", map->count, n_elements);

     for (unsigned int i = 0; i < n_elements; i++) {
        int key_int = rand () % ((n_elements * 2) - 1) + 1;;
        const void *key = &key_int;

        if (!htab_contains_key (map, key, sizeof (int))) {
            if (key_int > n_elements) printf ("[OKAY]: no data for key: %d\n", key_int);
            else printf ("[ERROR]: NO DATA FOR KEY: %d\n", key_int);
        }
    }

    htab_destroy (map);

    return 0;

}

int main (void) {

    srand ((unsigned) time (NULL));

    int errors = 0;

    errors |= test_int_insert ();

    errors |= test_int_remove ();

    errors |= test_int_get ();

    errors |= test_int_contains ();

    printf ("\nDone!\n");

    return errors;

}