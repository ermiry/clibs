#ifndef _CLIBS_FILES_H_
#define _CLIBS_FILES_H_

#include <stdio.h>

#include <sys/stat.h>

#include "clibs/types/types.h"
#include "clibs/types/string.h"

#include "clibs/collections/dlist.h"

// sanitizes a filename to correctly be used to save a file
// removes every character & whitespaces except for
// alphabet, numbers, '-', '_' and  '.'
extern void files_sanitize_filename (char *filename);

// check if a directory already exists, and if not, creates it
// returns 0 on success, 1 on error
extern unsigned int files_create_dir (
	const char *dir_path, mode_t mode
);

// returns an allocated string with the file extension
// NULL if no file extension
extern char *files_get_file_extension (const char *filename);

// returns a list of strings containg the names of all the files in the directory
extern DoubleList *files_get_from_dir (const char *dir);

// reads each one of the file's lines into newly created strings
// and returns them inside a dlist
extern DoubleList *file_get_lines (
	const char *filename, const size_t buffer_size
);

// returns true if the filename exists
extern bool file_exists (const char *filename);

// opens a file and returns it as a FILE
extern FILE *file_open_as_file (
	const char *filename,
	const char *modes, struct stat *filestatus
);

// opens and reads a file into a buffer
// sets file size to the amount of bytes read
extern char *file_read (
	const char *filename, size_t *file_size
);

// opens a file with the required flags
// returns fd on success, -1 on error
extern int file_open_as_fd (
	const char *filename, struct stat *filestatus, int flags
);

#endif