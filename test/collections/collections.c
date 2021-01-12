#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <time.h>

#include <clibs/utils/log.h>

#include "collections.h"

int main (int argc, char **argv) {

	srand ((unsigned) time (NULL));

	(void) printf ("Testing COLLECTIONS...\n");

	clibs_log_init ();

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			if (!strcmp (argv[i], "--quiet")) {
				clibs_log_set_quiet (true);
			}
		}
	}

	(void) collections_tests_dlist ();

	collections_tests_htab ();

	(void) printf ("\nDone with COLLECTIONS tests!\n\n");

	clibs_log_end ();

	return 0;

}