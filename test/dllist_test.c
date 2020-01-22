#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <pthread.h>

#include "../include/lists/dllist.h"

typedef struct { int value; } Integer;

int compare_int (const void *one, const void *two) {

	if (one && two) {
		Integer *int_a = (Integer *) one;
		Integer *int_b = (Integer *) two;

		if (int_a->value < int_b->value) return -1;
		else if (int_a->value == int_b->value) return 0;
		else return 1;
	}

}

static int test_remove (void) {

	DoubleList *list = dlist_init (NULL, compare_int);

	for (int i = 0; i < 1000; i++) {
		Integer *integer = (Integer *) malloc (sizeof (int));
		// integer->value = rand () % 99 + 1;
		integer->value = i;
		dlist_insert_after (list, dlist_end (list), integer);
	}

	printf ("\nItems in list: %ld\n", dlist_size (list));
	// for (ListElement *le = dlist_start (list); le != NULL; le = le->next) {
	// 	Integer *integer = (Integer *) le->data;
	// 	printf ("%3i", integer->value);
	// }

	Integer *query = (Integer *) malloc (sizeof (int));
	if (query) {
		query->value = 5;
		dlist_remove (list, query);
		free (query);
	}

	// printf ("\n\n");
	printf ("\nItems in list: %ld\n", dlist_size (list));
	// for (ListElement *le = dlist_start (list); le != NULL; le = le->next) {
	// 	Integer *integer = (Integer *) le->data;
	// 	printf ("%3i", integer->value);
	// }
	
	dlist_delete (list);

	return 0;

}

static int test_sort (void) {

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

	return 0;

}

static void *test_thread_add (void *args) {

	if (args) {
		DoubleList *list = (DoubleList *) args;

		// add ten items at the list end
		for (unsigned int i = 0; i < 1000; i++) {
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
		for (unsigned int i = 0; i < 500; i++) {
			dlist_remove_element (list, dlist_start (list));
		}
	}

}

static void *test_thread_search (void *args) {

	if (args) {
		DoubleList *list = (DoubleList *) args;

		// get 10 random values from the list
		for (unsigned int i = 0; i < 10; i++) {
			Integer *integer = (Integer *) malloc (sizeof (int));
			integer->value = rand () % 999 + 1;

			printf ("Searching: %d...\n", integer->value);
			Integer *search = (Integer *) dlist_search (list, integer);
			if (search) printf ("%d - %d\n", i + 1, search->value);

			free (integer);
		}
	}

}

static void *test_thread_get_element (void *args) {

	if (args) {
		DoubleList *list = (DoubleList *) args;

		// get 10 random values from the list
		for (unsigned int i = 0; i < 10; i++) {
			Integer *integer = (Integer *) malloc (sizeof (int));
			integer->value = rand () % 999 + 1;

			printf ("Getting element for: %d...\n", integer->value);
			ListElement *le = dlist_get_element (list, integer);
			Integer *search = (Integer *) le->data;
			if (search) printf ("%d - %d\n", i + 1, search->value);

			free (integer);
		}
	}

}

static int test_thread_safe (void) {

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
	pthread_create (&threads[1], NULL, test_thread_add, list);
	// pthread_create (&threads[2], NULL, test_thread_remove, list);
	// pthread_create (&threads[3], NULL, test_thread_remove, list);

	for (unsigned int i = 0; i < 2; i++) {
		pthread_join (threads[i], NULL);
	}

	// get how many items are on the list, we expect 40 if all add ten items
	printf ("\nItems in list: %ld\n", dlist_size (list));
	Integer *integer = NULL;
	// for (ListElement *le = dlist_start (list); le; le = le->next) {
	// 	integer = (Integer *) le->data;
	// 	printf ("%6i", integer->value);
	// }

	// 22/01/2020 -- 3:14 -- this has no problem
	pthread_create (&threads[0], NULL, test_thread_add, list);
	pthread_create (&threads[1], NULL, test_thread_remove, list);
	// pthread_create (&threads[2], NULL, test_thread_search, list);
	pthread_create (&threads[2], NULL, test_thread_get_element, list);
	pthread_join (threads[0], NULL);
	pthread_join (threads[1], NULL);
	pthread_join (threads[2], NULL);

	printf ("\nItems in list: %ld\n", dlist_size (list));

	// 21/01/2020 -- 15:09 -- we get a segfault every other time and NOT all items get inserted when that happens
	dlist_delete (list);

	return 0;

}

// uncomment the function that represents the test you want to run and the follow these steps
// from test directory...
// mkdir bin
// to compile run: gcc ./test/dllist_test.c ./src/lists/dllist.c -l pthread -o ./bin/dllist_test
// and run using: ./bin/dllist_test
int main (void) {

	srand ((unsigned) time (NULL));

	// return test_remove ();

	// return test_sort ();

	return test_thread_safe ();

	return 0;

}