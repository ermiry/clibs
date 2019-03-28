#include <string.h>
#include <ctype.h>
#include <stdio.h>

/*** STRINGS ***/

#pragma region STRINGS

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

char *str_toUpper (char *str) {

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

char *str_toLower (char *str) {

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

/*** OTHER ***/

#pragma region OTHER

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

#pragma endregion
