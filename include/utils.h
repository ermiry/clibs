#ifndef UTILS_H
#define UTILS_H

/*** STRINGS ***/

extern void str_copy (char *to, char *from);
extern void str_concat (char *des, char *s1, char *s2);
extern char *str_toUpper (char *str);
extern char *str_toLower (char *str);

/*** OTHER ***/

extern void cleanStdin (void);

#endif
