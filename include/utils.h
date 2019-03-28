#ifndef UTILS_H
#define UTILS_H

/*** STRINGS ***/

extern void str_copy (char *to, char *from);
extern void str_concat (char *des, char *s1, char *s2);
extern char *str_toUpper (char *str);
extern char *str_toLower (char *str);
extern char *str_create (const char *stringWithFormat, ...);

/*** OTHER ***/

// similar to fflush (stdin)
extern void clean_stdin (void);

// reads a line from stdin into buffer and returns the lenght
extern int read_line (char *buffer, int bsize);

#endif
