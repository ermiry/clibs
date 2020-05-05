#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <pthread.h>

#include "../include/collections/dllist.h"

typedef struct { int value; } Integer;

static Integer *integer_new (int value) {

	Integer *integer = (Integer *) malloc (sizeof (Integer));
	if (integer) integer->value = value;
	return integer;

}

static int compare_int (const void *one, const void *two) {

	if (one && two) {
		Integer *int_a = (Integer *) one;
		Integer *int_b = (Integer *) two;

		if (int_a->value < int_b->value) return -1;
		else if (int_a->value == int_b->value) return 0;
		else return 1;
	}

}

// 06/02/2020 -- 10:37
static int test_insert (void) {

	DoubleList *dlist = dlist_init (free, compare_int);

	// test insert at
	for (unsigned int i = 0; i < 10; i++) {
		Integer *integer = integer_new (i);
		dlist_insert_after (dlist, dlist_end (dlist), integer);
	}

	dlist_insert_at (dlist, integer_new (100), 0);

	for (ListElement *le = dlist_start (dlist); le; le = le->next) {
		printf ("%4d", ((Integer *) le->data)->value);
	}

	dlist_delete (dlist);

	return 0;

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
		Integer *found = dlist_remove (list, query, NULL);
		if (found) free (found);
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

static int test_insert_end_remove_start (void) {

	DoubleList *list = dlist_init (NULL, compare_int);

	for (int i = 0; i < 101; i++) {
		Integer *integer = (Integer *) malloc (sizeof (int));
		// integer->value = rand () % 99 + 1;
		integer->value = i;
		dlist_insert_after (list, dlist_end (list), integer);

		if (i < 100) {
			Integer *query = (Integer *) malloc (sizeof (int));
			query->value = i;

			free (dlist_remove (list, query, NULL));
			// free (dlist_remove_element (list, NULL));
		} 
	}

	printf ("\nRemaining list item: %d -- size: %ld\n", ((Integer *) list->start->data)->value, list->size);

	dlist_delete (list);

	return 0;

}

static int test_insert_end_remove_end (void) {

	DoubleList *list = dlist_init (NULL, compare_int);

	for (int i = 0; i < 101; i++) {
		Integer *integer = (Integer *) malloc (sizeof (int));
		// integer->value = rand () % 99 + 1;
		integer->value = i;
		dlist_insert_after (list, dlist_end (list), integer);

		if (i > 1 && i < 100) dlist_remove_element (list, list->end);
	}

	printf ("\nRemaining list item: %d -- size: %ld\n", ((Integer *) list->start->data)->value, list->size);

	dlist_delete (list);

	return 0;

}

static void test_traverse_method (void *list_element_data, void *method_args) {

	printf ("%4d", ((Integer *) list_element_data)->value);

}

static void *test_traverse_method_thread (void *args) {

	if (args) {
		DoubleList *list = (DoubleList *) args;

		dlist_traverse (list, test_traverse_method, NULL);
		printf ("\n\n");
	}

}

static int test_traverse (void) {

	int retval = 0;

	DoubleList *list = dlist_init (NULL, compare_int);

	for (int i = 0; i < 100; i++) {
		Integer *integer = (Integer *) malloc (sizeof (int));
		// integer->value = rand () % 99 + 1;
		integer->value = i;
		dlist_insert_after (list, dlist_end (list), integer);
	}

	// create 4 threads
	const unsigned int N_THREADS = 4;
	pthread_t threads[N_THREADS];

	pthread_create (&threads[0], NULL, test_traverse_method_thread, list);
	pthread_create (&threads[1], NULL, test_traverse_method_thread, list);
	pthread_create (&threads[2], NULL, test_traverse_method_thread, list);
	pthread_create (&threads[3], NULL, test_traverse_method_thread, list);
	pthread_join (threads[0], NULL);
	pthread_join (threads[1], NULL);
	pthread_join (threads[2], NULL);
	pthread_join (threads[3], NULL);

	// retval |= dlist_traverse (list, test_traverse_method, NULL);

	dlist_delete (list);

	return retval;

}

static int test_sort (void) {

	DoubleList *list = dlist_init (NULL, compare_int);

	for (int i = 0; i < 100; i++) {
		Integer *integer = (Integer *) malloc (sizeof (int));
		integer->value = rand () % 99 + 1;
		dlist_insert_after (list, dlist_start (list), integer);
	}

	dlist_sort (list, NULL);
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
		for (unsigned int i = 0; i < 100; i++) {
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
		for (unsigned int i = 0; i < 50; i++) {
			void *integer = dlist_remove_element (list, dlist_start (list));
			if (integer) free (integer);
		}
	}

}

static void *test_thread_search (void *args) {

	if (args) {
		DoubleList *list = (DoubleList *) args;

		// get 10 random values from the list
		for (unsigned int i = 0; i < 10; i++) {
			Integer *integer = (Integer *) malloc (sizeof (int));
			integer->value = rand () % 99 + 1;

			printf ("Searching: %d...\n", integer->value);
			Integer *search = (Integer *) dlist_search (list, integer, NULL);
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
			ListElement *le = dlist_get_element (list, integer, NULL);
			Integer *search = (Integer *) le->data;
			if (search) printf ("%d - %d\n", i + 1, search->value);

			free (integer);
		}
	}

}

static void *test_thread_sort (void *args) {

	if (args) {
		DoubleList *list = (DoubleList *) args;

		dlist_sort (list, NULL);
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
	pthread_create (&threads[2], NULL, test_thread_search, list);
	// pthread_create (&threads[2], NULL, test_thread_get_element, list);
	// pthread_create (&threads[3], NULL, test_thread_sort, list);
	pthread_join (threads[0], NULL);
	pthread_join (threads[1], NULL);
	pthread_join (threads[2], NULL);
	// pthread_join (threads[3], NULL);

	printf ("\nItems in list: %ld\n", dlist_size (list));
	dlist_sort (list, NULL);
	unsigned int i = 1;
	for (ListElement *le = dlist_start (list); le != NULL; le = le->next) {
		Integer *integer = (Integer *) le->data;
		printf ("%3d - %3i\n", i, integer->value);
		i++;
	}

	// 21/01/2020 -- 15:09 -- we get a segfault every other time and NOT all items get inserted when that happens
	dlist_delete (list);

	return 0;

}

static int test_get_at (void) {

	// create a global list
	DoubleList *list = dlist_init (NULL, compare_int);

	for (int i = 0; i < 100; i++) {
		Integer *integer = (Integer *) malloc (sizeof (int));
		integer->value = i;
		dlist_insert_after (list, dlist_end (list), integer);
	}

	// get 10 random values from the list
	for (unsigned int i = 0; i < 10; i++) {
		unsigned int idx = rand () % 999 + 1;

		printf ("Getting element at idx: %d... ", idx);
		// ListElement *le = dlist_get_element_at (list, idx);
		// if (le) {
		// 	Integer *integer = (Integer *) le->data;
		// 	printf ("%3d\n", integer->value);
		// }

		Integer *integer = (Integer *) dlist_get_at (list, idx);
		if (integer) printf ("%3d\n", integer->value);
	}

	dlist_delete (list);

	return 0;

}

static int test_remove_at (void) {

	// create a global list
	DoubleList *list = dlist_init (NULL, compare_int);

	for (int i = 0; i < 10; i++) {
		Integer *integer = (Integer *) malloc (sizeof (int));
		integer->value = i;
		dlist_insert_after (list, dlist_end (list), integer);
	}

	// print list
	for (ListElement *le = dlist_start (list); le != NULL; le = le->next) {
		Integer *integer = (Integer *) le->data;
		printf ("%3d ", integer->value);
	}

	// get 10 random values from the list
	// for (unsigned int i = 0; i < 5; i++) {
		// unsigned int idx = rand () % 9 + 1;
		unsigned idx = 7;

		// printf ("Removing element at idx: %d...\n", idx);
		// ListElement *le = dlist_get_element_at (list, idx);
		// if (le) {
		// 	Integer *integer = (Integer *) le->data;
		// 	printf ("%3d\n", integer->value);
		// }

		Integer *integer = (Integer *) dlist_remove_at (list, idx);
		// if (integer) printf ("%3d\n", integer->value);
	// }

	printf ("\n\n");

	// print list
	for (ListElement *le = dlist_start (list); le != NULL; le = le->next) {
		Integer *integer = (Integer *) le->data;
		printf ("%3d ", integer->value);
	}

	dlist_delete (list);

	return 0;

}

static int test_array (void) {

	int retval = 1;

	// create a global list
	DoubleList *list = dlist_init (NULL, compare_int);

	for (int i = 0; i < 10; i++) {
		Integer *integer = (Integer *) malloc (sizeof (int));
		integer->value = i;
		dlist_insert_after (list, dlist_end (list), integer);
	}

	printf ("Elements in list: \n");
	for (ListElement *le = dlist_start (list); le != NULL; le = le->next) {
		Integer *integer = (Integer *) le->data;
		printf ("%3d ", integer->value);
	}

	printf ("\n");

	size_t count = 0;
	void **array = dlist_to_array (list, &count);
	if (array) {
		dlist_clear (list);

		printf ("Elements in array: \n");
		for (size_t idx = 0; idx < count; idx++) {
			printf ("%3d ", ((Integer *) array[idx])->value);
		}

		printf ("\n\nList is %ld long\n", list->size);
		printf ("Array is %ld long\n", count);

		// clear array
		for (size_t idx = 0; idx < count; idx++) {
			free (array[idx]);
		}

		free (array);

		retval = 0;
	}

	dlist_delete (list);

	return retval;

}

static int test_empty (void) {

	int retval = 1;

	// create a global list
	DoubleList *list = dlist_init (NULL, compare_int);

	if (dlist_is_empty (list)) printf ("List is empty\n");
	if (dlist_is_not_empty (list)) printf ("List is not empty\n");

	for (int i = 0; i < 10; i++) {
		Integer *integer = (Integer *) malloc (sizeof (int));
		integer->value = i;
		dlist_insert_after (list, dlist_end (list), integer);
	}

	printf ("Elements in list: \n");
	for (ListElement *le = dlist_start (list); le != NULL; le = le->next) {
		Integer *integer = (Integer *) le->data;
		printf ("%3d ", integer->value);
	}

	printf ("\n");

	if (dlist_is_empty (list)) printf ("List is empty\n");
	if (dlist_is_not_empty (list)) printf ("List is not empty\n");

	dlist_delete (list);

	retval = 0;

	return retval;

}

static void *integer_clone (const void *original) {

	Integer *cloned_integer = NULL;

	if (original) {
		cloned_integer = integer_new (((Integer *) original)->value);
	}

	return cloned_integer;

}

static int test_clone (void) {

	int retval = 1;

	// create a global list
	DoubleList *list = dlist_init (NULL, compare_int);

	for (int i = 0; i < 10; i++) {
		Integer *integer = (Integer *) malloc (sizeof (int));
		integer->value = i;
		dlist_insert_after (list, dlist_end (list), integer);
	}

	DoubleList *clone = dlist_clone (list, integer_clone);

	printf ("Elements in original list: \n");
	for (ListElement *le = dlist_start (list); le != NULL; le = le->next) {
		Integer *integer = (Integer *) le->data;
		printf ("%3d ", integer->value);
	}

	printf ("\n");

	printf ("Elements in cloned list: \n");
	for (ListElement *le = dlist_start (clone); le != NULL; le = le->next) {
		Integer *integer = (Integer *) le->data;
		printf ("%3d ", integer->value);
	}

	printf ("\n");

	dlist_delete (list);
	dlist_delete (clone);

	retval = 0;

	return retval;

}

// uncomment the function that represents the test you want to run and the follow these steps
// from test directory...
// mkdir bin
// to compile run: gcc ./dllist_test.c ../src/collections/dllist.c -l pthread -o ./bin/dllist_test
// or compile all tests using the Makefile
// and run using: ./bin/dllist_test
int main (void) {

	srand ((unsigned) time (NULL));

	int res = 0;

	// res |= test_insert ();

	// res |= test_remove ();

	// res |= test_insert_end_remove_start ();

	// return test_insert_end_remove_end ();

	// res |= test_traverse ();

	// return test_sort ();

	// return test_thread_safe ();

	// return test_remove_at ();

	// res |= test_array ();

	// res |= test_empty ();

	res |= test_clone ();

	return res;

}