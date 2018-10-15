#include <string.h>
#include <ctype.h>

#pragma region STRINGS

/*** STRINGS ***/

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

    while (*str) *temp++ = toupper (*str++);

    *temp = '\0';

    return temp;

}

char *str_toLower (char *str) {

    if (!str) return NULL;

    char *temp = (char *) calloc (strlen (str) + 1, sizeof (char));

    while (*str) *temp++ = tolower (*str++);

    *temp = '\0';

    return temp;

}

#pragma endregion