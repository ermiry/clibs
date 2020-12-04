#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <pthread.h>

#include <clibs/collections/avl.h>

#include "user.h"

#pragma region user

static void avl_user_print (AVLNode *node) {

	if (node) {
		avl_user_print (node->right);

		if (node->id) printf ("Username: %s\n", ((User *) node->id)->name);

		avl_user_print (node->left);
	}

}

// insert some users into the tree, print, remove and search
static int simple_user_test (void) {

	printf ("\n[TEST]: Simple user test\n");

	// create and initialize to default values our tree
	AVLTree *tree = avl_init (user_comparator, user_delete);

	User *erick = user_new ("erick", 1);    // we create a new user 
	avl_insert_node (tree, erick);          // insert the user into the tree

	User *luis = user_new ("luis", 2);      // we create a new user 
	avl_insert_node (tree, luis);           // insert the user into the tree

	User *isai = user_new ("isai", 3);      // we create a new user 
	avl_insert_node (tree, isai);           // insert the user into the tree

	avl_user_print (tree->root);

	// we can remove a node from the tree by passing a reference to its data
	// and the tree will use the comparator method to do its magic and return 
	// the data ptr if it exists, if not, it will return NULL
	User *luis_two = (User *) avl_remove_node (tree, luis);
	user_delete (luis_two);                 // we msut delete this ptr

	// we can use the comparator method and create a new user that we will use a query
	// and get data from the tree
	User *query = user_new ("erick", 1);
	void *found_ptr = avl_get_node_data (tree, query, NULL);
	if (found_ptr) {
		User *found = (User *) found_ptr;
		printf ("\nFound name: %s\n", found->name);
	}

	else printf ("\nNot found!!!\n");

	user_delete (query);                    // the query must be deleted aftyer use

	avl_user_print (tree->root);

	// remember to delete the tree after use to avoid memory leaks
	avl_delete (tree);

	return 0;

}

#pragma endregion

typedef struct { int value; } Integer;

static int compare_int (const void *one, const void *two) {

	if (one && two) {
		Integer *int_a = (Integer *) one;
		Integer *int_b = (Integer *) two;

		if (int_a->value < int_b->value) return -1;
		else if (int_a->value == int_b->value) return 0;
		else return 1;
	}

	return 0;

}

static int print_count = 0;

static void avl_int_print (AVLNode *node) {

	if (node) {
		avl_int_print (node->left);
		
		if (node->id) {
			print_count++;
			printf ("%4d - %4d\n", print_count, ((Integer *) node->id)->value);
		}

		avl_int_print (node->right);		
	}

}

static void avl_int_print_count (AVLNode *node) {

	if (node) {
		avl_int_print_count (node->left);
		
		if (node->id) {
			print_count++;
		}

		avl_int_print_count (node->right);		
	}

}

// insert n random values and print them
static int test_insert_rand_values (void) {

	AVLTree *tree = avl_init (compare_int, free);

	for (unsigned int i = 0; i < 10; i++) {
		Integer *integer = (Integer *) malloc (sizeof (Integer));
		integer->value = rand () % 99 + 1;
		avl_insert_node (tree, integer);
	}

	avl_int_print (tree->root);

	avl_delete (tree);

	return 0;

}

static unsigned int test_thread_add_count = 1;

static void *test_thread_add (void *args) {

	if (args) {
		printf ("\n%d - test_thread_add ()\n", test_thread_add_count);
		test_thread_add_count++;

		AVLTree *tree = (AVLTree *) args;

		// add ten items at the list end
		for (unsigned int i = 0; i < 1000; i++) {
			Integer *integer = (Integer *) malloc (sizeof (int));
			// integer->value = rand () % 99 + 1;
			integer->value = i;
			avl_insert_node (tree, integer);
		}
	}

	return NULL;

}

static unsigned int test_thread_remove_count = 1;

static void *test_thread_remove (void *args) {

	if (args) {
		printf ("\n%d - test_thread_remove ()\n", test_thread_remove_count);
		test_thread_remove_count++;

		AVLTree *tree = (AVLTree *) args;

		// add ten items at the list end
		for (unsigned int i = 0; i < 1000; i++) {
			Integer *integer = (Integer *) malloc (sizeof (int));
			// integer->value = rand () % 99 + 1;
			integer->value = i;
			
			Integer *found = (Integer *) avl_remove_node (tree, integer);
			if (found) free (found);

			free (integer);
		}
	}

	return NULL;

}

static int test_thread_safe (void) {

	printf ("\n[TEST]: Thread safe test\n");

	AVLTree *tree = avl_init (compare_int, free);

	// create 4 threads
	const unsigned int N_THREADS = 4;
	pthread_t threads[N_THREADS];

	for (unsigned int i = 0; i < N_THREADS; i++) {
		pthread_create (&threads[i], NULL, test_thread_add, tree);
	}

	// join the threads
	for (unsigned int i = 0; i < N_THREADS; i++) {
		pthread_join (threads[i], NULL);
	}

	// pthread_create (&threads[0], NULL, test_thread_add, tree);
	// pthread_create (&threads[1], NULL, test_thread_add, tree);

	// for (unsigned int i = 0; i < 2; i++) {
	// 	pthread_join (threads[i], NULL);
	// }

	// after 4 threads inserting values, count must be 4000
	print_count = 1;
	// avl_int_print (tree->root);

	pthread_create (&threads[0], NULL, test_thread_add, tree);
	pthread_create (&threads[1], NULL, test_thread_remove, tree);

	pthread_join (threads[0], NULL);
	pthread_join (threads[1], NULL);

	// after another insert and one remove threa, count must be still 4000
	printf ("\nFINAL RESULT\n");
	print_count = 0;
	// avl_int_print (tree->root);
	avl_int_print_count (tree->root);
	printf ("\nFINAL COUNT: %d\n", print_count);

	avl_delete (tree);

	return 0;

}

// uncomment the function that represents the test you want to run and the follow these steps
// from the test directory
// mkdir bin
// to compile run: gcc ./avl_test.c ../src/collections/avl.c -l pthread -o ./bin/avl_test
// or compile all tests using the Makefile
// and run using: ./bin/avl_test
int main (void) {

	srand ((unsigned) time (NULL));

	// return simple_user_test ();

	// return test_insert_rand_values ();

	return test_thread_safe ();

	return 0;

}