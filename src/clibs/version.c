#include <stdio.h>

#include "clibs/version.h"

// print full erver version information 
void clibs_version_print_full (void) {

    printf ("\nClibs Version: %s\n", CLIBS_VERSION_NAME);
    printf ("Release Date & time: %s - %s\n", CLIBS_VERSION_DATE, CLIBS_VERSION_TIME);
    printf ("Author: %s\n\n", CLIBS_VERSION_AUTHOR);

}

// print the version id
void clibs_version_print_version_id (void) {

    printf ("\nClibs Version ID: %s\n", CLIBS_VERSION);

}

// print the version name
void clibs_version_print_version_name (void) {

    printf ("\nClibs Version: %s\n", CLIBS_VERSION_NAME);

}