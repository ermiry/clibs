#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/utils/utils.h"

int count_tokens (const char *string, const char delim) {

	printf ("\ncount_tokens () - before count <%s>\n", string);

	size_t count = c_string_count_tokens (string, delim);

	printf ("\ncount_tokens () - <%s> -> %ld tokens\n", string, count);

	printf ("\ncount_tokens () - after count <%s>\n\n", string);

	return 0;

}

int split (const char *string, const char delim) {

	printf ("\nsplit () - before split <%s>\n", string);

	size_t n_tokens = 0;
	char **tokens = c_string_split (string, delim, &n_tokens);
	if (tokens) {
		printf ("\nsplit () - <%s> -> %ld tokens\n", string, n_tokens);

		for (unsigned int i = 0; i < n_tokens; i++) {
			printf ("[%d] - %s\n", i, tokens[i]);
		}

		for (unsigned int i = 0; i < n_tokens; i++) free (tokens[i]);
		free (tokens);
	}

	else {
		printf ("split () - No tokens!\n");
	}

	printf ("\nsplit () - after split <%s>\n\n", string);

	return 0;

}

int reverse (const char *string) {

	printf ("\nreverse () - before reverse <%s>\n", string);

	char *r = c_string_reverse (string);
	if (r) {
		printf ("\n<%s>\n", r);
		free (r);
	}

	return 0;

}

int starts (const char *string, const char *sub) {

	if (c_string_starts_with (string, sub)) {
		printf ("\n%s starts with %s\n", string, sub);
	}

	else printf ("\n%s DOES NOT start with %s\n", string, sub);

	return 0;

}

int main (void) {

	int errors = 0;

	// "//hola///test//erick/"

	errors |= count_tokens ("/api/hola/test", '/');

	errors |= split ("/api/hola/test", '/');

	errors |= reverse ("hola a todos");

	errors |= starts ("hola me llamo erick", "hola");

	return errors;

}