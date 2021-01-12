#ifndef _CLIBS_STRING_H_
#define _CLIBS_STRING_H_

#include "clibs/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct String {

    unsigned int len;
    char *str;

} String;

extern String *str_new (const char *str);

extern void str_delete (void *str_ptr);

extern String *str_create (const char *format, ...);

extern int str_compare (const String *s1, const String *s2);

extern int str_comparator (const void *a, const void *b);

extern void str_copy (String *to, String *from);

extern void str_replace (String *old, const char *str);

// concatenates two strings into a new one
extern String *str_concat (String *s1, String *s2);

// appends a char to the end of the string
// reallocates the same string
extern void str_append_char (String *s, const char c);

// appends a c string at the end of the string
// reallocates the same string
extern void str_append_c_string (String *s, const char *c_str);

extern void str_to_upper (String *string);

extern void str_to_lower (String *string);

extern char **str_split (
    String *string, const char delim, int *n_tokens
);

extern void str_remove_char (String *string, char garbage);

// removes the last char from a string
extern void str_remove_last_char (String *string);

// check if a str (to_find) is inside str
// returns 0 on exact match
// returns 1 if it match the letters but len is different
// returns -1 if no match
extern int str_contains (String *str, char *to_find);

#ifdef __cplusplus
}
#endif

#endif