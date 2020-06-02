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

int test_int_insert_remove (void) {

    printf ("\ntest_int_insert_remove\n");

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

    unsigned int removed = 0;
    for (unsigned int i = 0; i < n_elements; i++) {
        // const void *key = &i;
        int key_int = rand () % 99 + 1;
        printf ("key_int: %d\n", key_int);
        const void *key = &key_int;

        Data *data = (Data *) htab_remove (map, key, sizeof (int));
        if (data) {
            // data_print (data);
            data_delete (data);
            removed += 1;
        }
    }

    printf ("\nN elements removed: %d\n", removed);

    printf ("\nItems in map after remove: %ld / %d\n", map->count, n_elements);

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

typedef struct ThreadData {

    unsigned int id;
    Htab *map;

} ThreadData;

ThreadData *thread_data_new (int id, Htab *map) {

    ThreadData *data = (ThreadData *) malloc (sizeof (ThreadData));
    data->id = id;
    data->map = map;

}

void *int_thread_insert (void *data_ptr) {

    ThreadData *thread_data = (ThreadData *) data_ptr;

    int jump = 3;

    int inserted = 0;
    for (unsigned int i = thread_data->id; i < 300; i += jump) {
        const void *key = &i;

        Data *data = (Data *) malloc (sizeof (Data));
        data->idx = i;
        // data->value = rand () % 99 + 1;
        data->value = i;
        // value++;

        if (!htab_insert (
            thread_data->map, 
            key, sizeof (int), 
            data, sizeof (Data)
        )) {
            inserted += 1;
        }

        else {

        }
    }

    printf ("\nthread %d N elements inserted: %d\n", thread_data->id, inserted);

    return NULL;

}

void *int_thread_remove (void *data_ptr) {

    ThreadData *thread_data = (ThreadData *) data_ptr;

    unsigned int removed = 0;

    for (unsigned int i = thread_data->id; i < 100; i++) {
        // const void *key = &i;
        int key_int = rand () % 299 + 1;
        // printf ("key_int: %d\n", key_int);
        const void *key = &key_int;

        Data *data = (Data *) htab_remove (thread_data->map, key, sizeof (int));
        if (data) {
            // data_print (data);
            data_delete (data);
            removed += 1;
        }

        // else {
        //     printf ("\n\nNO DATA FOR KEY: %d\n\n", i);
        // }
    }

    printf ("\nN elements removed: %d\n", removed);

    return NULL;

}

int test_int_thread_insert_remove (void) {

    printf ("\ntest_int_thread_insert\n");

    int n_elements = 300;

    Htab *map = htab_create (
        HTAB_DEFAULT_INIT_SIZE,
        NULL,
        NULL,
        data_delete
    );

    const unsigned int N_THREADS = 4;
	pthread_t threads[N_THREADS];

    ThreadData *data_1 = thread_data_new (0, map);
    ThreadData *data_2 = thread_data_new (1, map);
    ThreadData *data_3 = thread_data_new (2, map);
    ThreadData *data_4 = thread_data_new (3, map);
	pthread_create (&threads[0], NULL, int_thread_insert, data_1);
	pthread_create (&threads[1], NULL, int_thread_insert, data_2);
	pthread_create (&threads[2], NULL, int_thread_insert, data_3);
	pthread_create (&threads[3], NULL, int_thread_remove, data_4);
	pthread_join (threads[0], NULL);
	pthread_join (threads[1], NULL);
	pthread_join (threads[2], NULL);
	pthread_join (threads[3], NULL);
    free (data_1);
    free (data_2);
    free (data_3);
    free (data_4);

    printf ("\nItems in map after insert & remove: %ld / %d\n", htab_size (map), n_elements);

    size_t count = 0;
    for (unsigned int i = 0; i < map->size; i++)
        count += map->table[i]->count;

    printf ("Bucket count: %ld\n", count);

    // htab_print (map);

    htab_destroy (map);

    return 0;

}

int main (void) {

    srand ((unsigned) time (NULL));

    int errors = 0;

    // errors |= test_int_insert ();

    // errors |= test_int_remove ();

    // errors |= test_int_insert_remove ();

    // errors |= test_int_get ();

    // errors |= test_int_contains ();

    errors |= test_int_thread_insert_remove ();

    printf ("\nDone!\n");

    return errors;

}