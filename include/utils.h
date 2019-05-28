#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdbool.h>

/*** Math ***/

extern int clamp_int (int val, int min, int max);
extern int abs_int (int value);
extern float lerp (float first, float second, float by);

/*** Random ***/

// init psuedo random generator based on our seed
extern void random_set_seed (unsigned int seed);
extern int random_int_in_range (int min, int max);

/*** Converters ***/

// convert a string representing a hex to a string
extern int xtoi (char *hexString);
extern char *itoa (int i, char b[]);

/*** STRINGS ***/

extern void str_copy (char *to, char *from);
extern void str_concat (char *des, char *s1, char *s2);
extern char *str_to_upper (char *str);
extern char *str_to_lower (char *str);
extern char *str_create (const char *stringWithFormat, ...);

/*** OTHER ***/

// similar to fflush (stdin)
extern void clean_stdin (void);

// reads a line from stdin into buffer and returns the lenght
extern int read_line (char *buffer, int bsize);

extern bool system_is_little_endian (void);

#endif
