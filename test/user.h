#ifndef _USER_H_
#define _USER_H_

typedef struct User {

    char *name;
    int id;

} User;

extern User *user_new (char *name, int id);

extern void user_delete (void *user_ptr);

extern int user_comparator (const void *user_ptr_a, const void *user_ptr_b);

#endif