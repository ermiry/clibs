#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <pthread.h>

#include "../include/threads/thpool.h"

typedef struct Data {

    unsigned int id;
    size_t max_num;

} Data;

Data *data_new (int id, size_t max_num) {

    Data *data = (Data *) malloc (sizeof (Data));
    if (data) {
        data->id = id;
        data->max_num = max_num;
    }

    return data;

}

void thread_count (void *data_ptr) {

    if (data_ptr) {
        Data *data = (Data *) data_ptr;

        size_t count = 0;
        for (size_t i = 0; i < data->max_num; i++) {
            count += 1;
        }

        printf ("\nThread: %d - count: %ld\n", data->id, count);
    }

}

int main (void) {

    threadpool thpool = thpool_init (4);

    for (unsigned int i = 1; i < 11; i++) {
         Data *data = data_new (i, 1000);
        thpool_add_work (thpool, thread_count, data);
    }

    // wait for all threads to finish
    printf ("\n\nWaiting 3 secs...\n");
    sleep (3);

    thpool_destroy (thpool);

    printf ("\nDone!\n");

    return 0;

}