#ifndef _CLIBS_VERSION_H_
#define _CLIBS_VERSION_H_

#define CLIBS_VERSION						"1.0"
#define CLIBS_VERSION_NAME					"Release 1.0"
#define CLIBS_VERSION_DATE					"03/12/2020"
#define CLIBS_VERSION_TIME					"21:41 CST"
#define CLIBS_VERSION_AUTHOR				"Erick Salas"

// print full clibs version information
extern void clibs_version_print_full (void);

// print the version id
extern void clibs_version_print_version_id (void);

// print the version name
extern void clibs_version_print_version_name (void);

#endif