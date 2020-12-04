#ifndef _CLIBS_INPUT_H_
#define _CLIBS_INPUT_H_

extern void input_clean_stdin (void);

// returns a newly allocated c string
extern char *input_get_line (void);

extern unsigned int input_password (char *password);

#endif