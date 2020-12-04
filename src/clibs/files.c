#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#define _XOPEN_SOURCE 700
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

#include "clibs/types/types.h"
#include "clibs/types/string.h"

#include "clibs/collections/dlist.h"

#include "clibs/files.h"

#include "clibs/threads/thread.h"

#include "clibs/utils/utils.h"
#include "clibs/utils/log.h"

// sanitizes a filename to correctly be used to save a file
// removes every character & whitespaces except for
// alphabet, numbers, '-', '_' and  '.'
void files_sanitize_filename (char *filename) {

	if (filename) {
		for (int i = 0, j; filename[i] != '\0'; ++i) {
			while (
				!(filename[i] >= 'a' && filename[i] <= 'z') && !(filename[i] >= 'A' && filename[i] <= 'Z')	// alphabet
				&& !(filename[i] >= 48 && filename[i] <= 57)												// numbers
				&& !(filename[i] == '-') && !(filename[i] == '_') && !(filename[i] == '.')					// clean characters
				&& !(filename[i] == '\0')
			) {
				for (j = i; filename[j] != '\0'; ++j) {
					filename[j] = filename[j + 1];
				}

				filename[j] = '\0';
			}
		}

		c_string_remove_spaces (filename);
	}

}

// check if a directory already exists, and if not, creates it
// returns 0 on success, 1 on error
unsigned int files_create_dir (
	const char *dir_path, mode_t mode
) {

	unsigned int retval = 1;

	if (dir_path) {
		struct stat st = { 0 };
		int ret = stat (dir_path, &st);
		switch (ret) {
			case -1: {
				if (!mkdir (dir_path, mode)) {
					retval = 0;		// success
				}

				else {
					clibs_log_error ("Failed to create dir %s!", dir_path);
				}
			} break;
			case 0: {
				clibs_log_warning ("Dir %s already exists!", dir_path);
			} break;

			default: break;
		}
	}

	return retval;

}

// returns an allocated string with the file extension
// NULL if no file extension
char *files_get_file_extension (const char *filename) {

	char *retval = NULL;

	if (filename) {
		char *ptr = strrchr ((char *) filename, '.');
		if (ptr) {
			// *ptr++;
			size_t ext_len = 0;
			char *p = ptr;
			while (*p++) ext_len++;

			if (ext_len) {
				retval = (char *) calloc (ext_len + 1, sizeof (char));
				if (retval) {
					memcpy (retval, ptr + 1, ext_len);
					retval[ext_len] = '\0';
				}
			}
		}

	}

	return retval;

}

// returns a list of strings containg the names of all the files in the directory
DoubleList *files_get_from_dir (const char *dir) {

	DoubleList *images = NULL;

	if (dir) {
		images = dlist_init (str_delete, str_comparator);

		DIR *dp = opendir (dir);
		if (dp) {
			struct dirent *ep = NULL;
			String *file = NULL;
			while ((ep = readdir (dp)) != NULL) {
				if (strcmp (ep->d_name, ".") && strcmp (ep->d_name, "..")) {
					file = str_create ("%s/%s", dir, ep->d_name);

					dlist_insert_after (images, dlist_end (images), file);
				}
			}

			(void) closedir (dp);
		}

		else {
			clibs_log_error ("Failed to open dir %s", dir);
		}
	}

	return images;

}

static String *file_get_line (
	FILE *file,
	char *buffer, const size_t buffer_size
) {

	String *str = NULL;

	if (!feof (file)) {
		if (fgets (buffer, buffer_size, file)) {
			c_string_remove_line_breaks (buffer);
			str = str_new (buffer);
		}
	}

	return str;

}

// reads each one of the file's lines into newly created strings
// and returns them inside a dlist
DoubleList *file_get_lines (
	const char *filename, const size_t buffer_size
) {

	DoubleList *lines = NULL;

	if (filename) {
		FILE *file = fopen (filename, "r");
		if (file) {
			lines = dlist_init (str_delete, str_comparator);

			char *buffer = (char *) calloc (buffer_size, sizeof (char));
			if (buffer) {
				String *line = NULL;

				while ((line = file_get_line (file, buffer, buffer_size))) {
					dlist_insert_at_end_unsafe (lines, line);
				}

				free (buffer);
			}

			(void) fclose (file);
		}

		else {
			clibs_log_error ("Failed to open file: %s", filename);
		}
	}

	return lines;

}

// returns true if the filename exists
bool file_exists (const char *filename) {

	bool retval = false;

	if (filename) {
		struct stat filestatus = { 0 };
		if (!stat (filename, &filestatus)) {
			retval = true;
		}
	}

	return retval;

}

// opens a file and returns it as a FILE
FILE *file_open_as_file (const char *filename, const char *modes, struct stat *filestatus) {

	FILE *fp = NULL;

	if (filename) {
		memset (filestatus, 0, sizeof (struct stat));
		if (!stat (filename, filestatus))
			fp = fopen (filename, modes);

		else {
			#ifdef FILES_DEBUG
			clibs_log (
				LOG_TYPE_ERROR, LOG_TYPE_FILE,
				"File %s not found!", filename
			);
			#endif
		}
	}

	return fp;

}

// opens and reads a file into a buffer
// sets file size to the amount of bytes read
char *file_read (const char *filename, size_t *file_size) {

	char *file_contents = NULL;

	if (filename) {
		struct stat filestatus = { 0 };
		FILE *fp = file_open_as_file (filename, "rt", &filestatus);
		if (fp) {
			*file_size = filestatus.st_size;
			file_contents = (char *) malloc (filestatus.st_size);

			// read the entire file into the buffer
			if (fread (file_contents, filestatus.st_size, 1, fp) != 1) {
				#ifdef FILES_DEBUG
				clibs_log (
					LOG_TYPE_ERROR, LOG_TYPE_FILE,
					"Failed to read file (%s) contents!", filename
				);
				#endif

				free (file_contents);
			}

			fclose (fp);
		}

		else {
			#ifdef FILES_DEBUG
			clibs_log (
				LOG_TYPE_ERROR, LOG_TYPE_FILE,
				"Unable to open file %s.", filename
			);
			#endif
		}
	}

	return file_contents;

}

// opens a file with the required flags
// returns fd on success, -1 on error
int file_open_as_fd (
	const char *filename, struct stat *filestatus, int flags
) {

	int retval = -1;

	if (filename) {
		memset (filestatus, 0, sizeof (struct stat));
		if (!stat (filename, filestatus)) {
			retval = open (filename, flags);
		}
	}

	return retval;

}