#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <pthread.h>

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

static void *test_thread_add (void *args) {

	if (args) {
		DoubleList *list = (DoubleList *) args;

		// add ten items at the list end
		for (unsigned int i = 0; i < 10; i++) {
			Integer *integer = (Integer *) malloc (sizeof (int));
			// integer->value = rand () % 99 + 1;
			integer->value = i;
			dlist_insert_after (list, dlist_end (list), integer);
		}
	}

}

static void *test_thread_remove (void *args) {

	if (args) {
		DoubleList *list = (DoubleList *) args;

		// remove 5 items from the start of the list
		for (unsigned int i = 0; i < 5; i++) {
			dlist_remove_element (list, dlist_start (list));
		}
	}

}

static void test_thread_safe (void) {

	// create a global list
	DoubleList *list = dlist_init (NULL, compare_int);

	// create 4 threads
	const unsigned int N_THREADS = 4;
	pthread_t threads[N_THREADS];
	// for (unsigned int i = 0; i < N_THREADS; i++) {
	// 	pthread_create (&threads[i], NULL, test_thread_add, list);
	// }

	// // join the threads
	// for (unsigned int i = 0; i < N_THREADS; i++) {
	// 	pthread_join (threads[i], NULL);
	// }

	// 21/01/2020 -- 15:15 -- some times getting seg fault, other we get wrong list size,
	// and other times we are printing a wrong number of items
	// the correct values should be size: 10 and ten integers getting printed to the console
	pthread_create (&threads[0], NULL, test_thread_add, list);
	pthread_create (&threads[1], NULL, test_thread_remove, list);
	pthread_create (&threads[2], NULL, test_thread_add, list);
	pthread_create (&threads[3], NULL, test_thread_remove, list);

	// get how many items are on the list, we expect 40 if all add ten items
	printf ("\nActual items in list: %ld\n", dlist_size (list));
	Integer *integer = NULL;
	for (ListElement *le = dlist_start (list); le; le = le->next) {
		integer = (Integer *) le->data;
		printf ("%3i", integer->value);
	}

	// 21/01/2020 -- 15:09 -- we get a segfault every other time and NOT all items get inserted when that happens
	dlist_delete (list);

}

// uncomment the function that represents the test you want to run and the follow these steps
// from test directory...
// mkdir bin
// to compile run: gcc ./test/dllist_test.c ./src/lists/dllist.c -l pthread -o ./bin/dllist_test
// and run using: ./bin/dllist_test
int main (void) {

	srand ((unsigned) time (NULL));

	// test_sort ();

	test_thread_safe ();

	return 0;

}