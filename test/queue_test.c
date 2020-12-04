#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <clibs/collections/queue.h>

typedef struct Data {

	int idx;
	int value;

} Data;

// static inline Data *data_new (int idx, int value) {

//     Data *data = (Data *) malloc (sizeof (Data));
//     if (data) {
//         data->idx = idx;
//         data->value = value;
//     }

//     return data;

// }

static inline void data_delete (void *data_ptr) {

	if (data_ptr) free (data_ptr);

}

static void queue_data_print_idx (Queue *queue) {

	if (queue) {
		Data *data = NULL;
		for (ListElement *le = dlist_start (queue->dlist); le; le = le->next) {
			data = (Data *) le->data;
			printf ("%4d", data->idx);
		}
	}

}

int test_int_insert_remove (void) {

	printf ("\ntest_int_insert_remove ()\n");

	Queue *queue = queue_create (data_delete);

	int value = 1000;
	for (unsigned int i = 0; i < 10; i++) {
		Data *data = (Data *) malloc (sizeof (Data));
		data->idx = i;
		// data->value = rand () % 99 + 1;
		data->value = value;
		value++;

		queue_push (queue, data);
	}

	printf ("\nAfter first insert: \n");
	queue_data_print_idx (queue);
	printf ("\n");

	for (unsigned int i = 10; i < 20; i++) {
		Data *data = (Data *) malloc (sizeof (Data));
		data->idx = i;
		// data->value = rand () % 99 + 1;
		data->value = value;
		value++;

		queue_push (queue, data);
	}

	printf ("\nAfter second insert: \n");
	queue_data_print_idx (queue);
	printf ("\n");

	for (unsigned int i = 0; i < 10; i++) {
		data_delete (queue_pop (queue));
	}

	printf ("\nAfter remove: \n");
	queue_data_print_idx (queue);
	printf ("\n");

	queue_delete (queue);

	return 0;

}

int test_int_remove (void) {

	printf ("\ntest_int_remove ()\n");

	int n_elements = 100;

	Queue *queue = queue_create (data_delete);

	int value = 1000;
	for (unsigned int i = 0; i < n_elements; i++) {
		Data *data = (Data *) malloc (sizeof (Data));
		data->idx = i;
		// data->value = rand () % 99 + 1;
		data->value = value;
		value++;

		queue_push (queue, data);
	}

	size_t items_count = queue_size (queue);
	printf ("\nItems in queue after insert: %ld / %d\n", items_count, n_elements);

	for (unsigned int i = 0; i < n_elements; i++) {
		data_delete (queue_pop (queue));
	}

	items_count = queue_size (queue);
	printf ("\nItems in queue after remove: %ld / 0\n", items_count);

	queue_delete (queue);

	return 0;

}

int main (void) {

	srand ((unsigned) time (NULL));

	int errors = 0;

	errors |= test_int_insert_remove ();

	errors |= test_int_remove ();

	printf ("\nDone!\n");

	return errors;

}