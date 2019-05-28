#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>

/*** MATH ***/

#pragma region Math

int clamp_int (int val, int min, int max) {

    const int t = val < min ? min : val;
    return t > max ? max : t;

}

int abs_int (int value) { return value > 0 ? value : (value * -1); }

float lerp (float first, float second, float by) { return first * (1 - by) + second * by; }

#pragma endregion

/*** Random ***/

#pragma region Random

// init psuedo random generator based on our seed
void random_set_seed (unsigned int seed) { srand (seed); }

int random_int_in_range (int min, int max) {

    int low = 0, high = 0;

    if (min < max) {
        low = min;
        high = max + 1;
    }

    else {
        low = max + 1;
        high = min;
    }

    return (rand () % (high - low)) + low;

}

#pragma endregion

/*** Converters ***/

#pragma region Converters

// convert a string representing a hex to a string
int xtoi (char *hexString) {

    int i = 0;

    if ((*hexString == '0') && (*(hexString + 1) == 'x')) hexString += 2;

    while (*hexString) {
        char c = toupper (*hexString++);
        if ((c < '0') || (c > 'F') || ((c > '9') && (c < 'A'))) break;
        c -= '0';
        if (c > 9) c-= 7;
        i = (i << 4) + c;
    }

    return i;

}

char *itoa (int i, char b[]) {

    char const digit[] = "0123456789";
    char *p = b;

    if (i < 0) {
        *p++ = '-';
        i *= -1;
    }

    int shifter = i;
    do { //Move to where representation ends
        ++p;
        shifter = shifter / 10;
    } while (shifter);

    *p = '\0';
    do { //Move back, inserting digits as u go
        *--p = digit [i % 10];
        i = i / 10;
    } while (i);

    return b;

}

#pragma endregion

/*** C Strings ***/

#pragma region C Strings

void str_copy (char *to, char *from) {

    while (*from)
        *to++ = *from++;

    *to = '\0';

}

void str_concat (char *des, char *s1, char *s2) {

    while (*s1) *des++ = *s1++;

    while (*s2) *des++ = *s2++;

    *des = '\0';

}

char *str_to_upper (char *str) {

    if (!str) return NULL;

    char *temp = (char *) calloc (strlen (str) + 1, sizeof (char));

    int i = 0;
    while (str[i]) {
        temp[i] = toupper (str[i]);
        i++;
    }

    temp[i] = '\0';

    return temp;

}

char *str_to_lower (char *str) {

    if (!str) return NULL;

    char *temp = (char *) calloc (strlen (str) + 1, sizeof (char));

    int i = 0;
    while (str[i]) {
        temp[i] = tolower (str[i]);
        i++;
    }

    temp[i] = '\0';

    return temp;

}

#include <stdarg.h>

char *str_create (const char *stringWithFormat, ...) {

    char *fmt;

    if (stringWithFormat != NULL) fmt = strdup (stringWithFormat);
    else fmt = strdup ("");

    va_list argp;
    va_start (argp, stringWithFormat);
    char oneChar[1];
    int len = vsnprintf (oneChar, 1, fmt, argp);
    if (len < 1) return NULL;
    va_end (argp);

    char *str = (char *) calloc (len + 1, sizeof (char));
    if (!str) return NULL;

    va_start (argp, stringWithFormat);
    vsnprintf (str, len + 1, fmt, argp);
    va_end (argp);

    free (fmt);

    return str;

}

#pragma endregion

/*** Other ***/

#pragma region Other

void clean_stdin (void) {

    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);

}

// reads a line from stdin into buffer and returns the lenght
int read_line (char *buffer, int bsize) {

    int ch, len;

    fgets (buffer, bsize, stdin);

    // remove unwanted characters from the buffer
    buffer[strcspn (buffer, "\r\n")] = '\0';

    len = strlen (buffer);

    // clean input buffer if needed
    if (len == bsize - 1) while ((ch = getchar ()) != '\n' && ch != EOF);

    return len;

}

bool system_is_little_endian (void) {

    unsigned int x = 0x76543210;
    char *c = (char *) &x;
    if (*c == 0x10) return true;
    else return false;

}

#pragma endregion