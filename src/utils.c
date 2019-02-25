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

#pragma endregion

/*** OTHER ***/

#pragma region OTHER

void cleanStdin (void) {

    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);

}

#pragma endregion
