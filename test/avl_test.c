#include <stdlib.h>
#include <stdio.h>

#include "../include/collections/avl.h"

#include <stdlib.h>
#include <string.h>

typedef struct User {

    char *name;
    int id;

} User;

static User *user_new (char *name, int id) {

    User *user = (User *) malloc (sizeof (User));
    if (user) {
        if (name) {
            size_t len = strlen (name);
            user->name = (char *) calloc (len + 1, sizeof (char));
            strcpy (user->name, name);
        }

        else user->name = NULL;
        user->id = id;
    }

    return user;

}

static inline void user_delete (void *user_ptr) {

    if (user_ptr) {
        User *user = (User *) user_ptr;
        if (user->name) free (user->name);
        free (user_ptr);
    }

}

static int user_comparator (const void *user_ptr_a, const void *user_ptr_b) {

    User *user_a = (User *) user_ptr_a;
    User *user_b = (User *) user_ptr_b;

    if (user_a->id < user_b->id) return -1; 
    if (user_a->id == user_b->id) return 0; 
    else return 1;

}

void avl_print (AVLNode *node) {

	if (node) {
		avl_print (node->right);

		if (node->id) printf ("Username: %s\n", ((User *) node->id)->name);

		avl_print (node->left);
	}

}

// uncomment the function that represents the test you want to run and the follow these steps
// from main directory...
// mkdir bin
// to compile run: gcc ./test/avl_test.c ./src/collections/avl.c -l pthread -o ./bin/avl_test
// and run using: ./bin/dllist_test
int main (void) {

	// create and initialize to default values our tree
	AVLTree *tree = avl_init (user_comparator, user_delete);

	User *erick = user_new ("erick", 1);    // we create a new user 
	avl_insert_node (tree, erick);          // insert the user into the tree

	User *luis = user_new ("luis", 2);      // we create a new user 
	avl_insert_node (tree, luis);           // insert the user into the tree

	User *isai = user_new ("isai", 3);      // we create a new user 
	avl_insert_node (tree, isai);           // insert the user into the tree

	avl_print (tree->root);

	// we can remove a node from the tree by passing a reference to its data
	// and the tree will use the comparator method to do its magic and return 
	// the data ptr if it exists, if not, it will return NULL
	User *luis_two = (User *) avl_remove_node (tree, luis);
	user_delete (luis_two);                 // we msut delete this ptr

	// we can use the comparator method and create a new user that we will use a query
	// and get data from the tree
	User *query = user_new ("erick", 2);
	void *found_ptr = avl_get_node_data (tree, query);
	if (found_ptr) {
		User *found = (User *) found_ptr;
		printf ("\nFound name: %s\n", found->name);
	}

	else printf ("\nNot found!!!\n");

	user_delete (query);                    // the query must be deleted aftyer use

	avl_print (tree->root);

	// remember to delete the tree after use to avoid memory leaks
	avl_delete (tree);

	return 0;

}