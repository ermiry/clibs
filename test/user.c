#include <stdlib.h>
#include <string.h>

#include "user.h"

User *user_new (char *name, int id) {

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

void user_delete (void *user_ptr) {

    if (user_ptr) {
        User *user = (User *) user_ptr;
        if (user->name) free (user->name);
        free (user_ptr);
    }

}

int user_comparator (const void *user_ptr_a, const void *user_ptr_b) {

    User *user_a = (User *) user_ptr_a;
    User *user_b = (User *) user_ptr_b;

    if (user_a->id < user_b->id) return -1; 
    if (user_a->id == user_b->id) return 0; 
    else return 1;

}